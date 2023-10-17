//
// Created by root on 10/9/23.
//
#include "include/RpcProvider.h"
#include "RpcHeader.pb.h"


void RpcProvider::NotifyService(google::protobuf::Service *service) {
    ServiceInfo serviceInfo;
    const google::protobuf::ServiceDescriptor *descriptor = service->GetDescriptor(); // 获取传入service对象的描述符
    std::string service_name = descriptor->name();
    serviceInfo.m_service = service;
    for (int i = 0; i < descriptor->method_count(); ++i) {
        const google::protobuf::MethodDescriptor *methodDescriptor = descriptor->method(i);
        std::string method_name = methodDescriptor->name();
        serviceInfo.m_methodMap.insert({method_name, methodDescriptor});
    }
    m_serviceMap.insert({service_name, serviceInfo});
    std::cout << "service_name:" << service_name << std::endl;
}

void RpcProvider::run() {
    // muduo网络库初始化
    std::string ip = MyrpcApplication::getInstance().getConfig().Load("rpcserverip");
    uint16_t port = atoi(MyrpcApplication::getInstance().getConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer server(&m_mainLoop, address, "rpcProvider");
    // tcpserver设置
    server.setThreadNum(8);
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3));

    // 启动网络服务
    server.start();
    m_mainLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) { // 连接丢失
        conn->shutdown();
    }
}


/*
 * 当新rpc请求到来时，负责
 *              1. 解析请求 2. 传递请求到本地服务  3. 生成响应  4. 发送响应给调用方
 *
 * 此处的请求是protobuf封装的二进制形式，由于是tcp传输，回出现粘包问题，于是约定请求报文如下
 * header_size(4 bytes) + header_str + arg_str
 * 其中服务名称和方法名称都包含在头字符串中，只需要把头字符串反序列化就可以得到
 */
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp) {
    std::string recv_buf = buffer->retrieveAllAsString(); // 接受所有字符

    uint32_t header_size;
    recv_buf.copy((char *) &header_size, 4); // 将收到数据的前四个字节解析为请求头长度

    // 反序列化请求
    myrpc::RpcHeader rpcHeader;
    std::string header_str = recv_buf.substr(4, header_size);
    if (!rpcHeader.ParseFromString(header_str)) {
        std::cout << "Parse Request Fatal!" << std::endl;
        return;
    }
    // 获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size, rpcHeader.args_size());

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << header_str << std::endl;
    std::cout << "service_name: " << rpcHeader.service_name() << std::endl;
    std::cout << "method_name: " << rpcHeader.method_name() << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;

    auto it = m_serviceMap.find(rpcHeader.service_name());
    if (it == m_serviceMap.end()) {
        std::cout << "Can Not Find Service!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(rpcHeader.method_name());
    if (mit == it->second.m_methodMap.end()) {
        std::cout << "Can Not Find Method!" << std::endl;
        return;
    }

    // 获取service和method
    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    // 生成本地请求request和响应response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 生成调用服务后的回调函数done
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(
            this, &RpcProvider::SendRpcResponse, conn, response);

    // 调用本地service的method
    service->CallMethod(method, nullptr, request, response, done);

}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response) {
    std::string response_str;
    if (response->SerializeToString(&response_str)) // response进行序列化
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}