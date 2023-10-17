//
// Created by root on 10/13/23.
//

#include "MyRpcChannel.h"
#include <arpa/inet.h>
#include <string>

void
MyRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller,
                         const google::protobuf::Message *request, google::protobuf::Message *response,
                         google::protobuf::Closure *done) {
    std::string service_name = method->service()->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("serialize request error!");
        return;
    }

    myrpc::RpcHeader header;
    header.set_service_name(service_name);
    header.set_method_name(method_name);
    header.set_args_size(args_size);
    uint32_t header_size = 0;
    std::string header_str;
    if (header.SerializeToString(&header_str)) {
        header_size = header_str.size();
    } else {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    std::string send_str;
    send_str.insert(0, std::string((char *) &header_size, 4)); // header_size
    send_str += header_str;
    send_str += args_str;

    // 打印调试信息
    std::cout << "============================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "============================================" << std::endl;


    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd) {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12304);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // 连接rpc服务节点
    if (-1 == connect(clientfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 发送rpc请求
    if (-1 == send(clientfd, send_str.c_str(), send_str.size(), 0)) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 接收rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size); // bug出现问题，recv_buf中遇到\0后面的数据就存不下来了，导致反序列化失败
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        close(clientfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);

}