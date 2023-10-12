//
// Created by root on 10/9/23.
// 用于发布rpc网络服务的对象类
//

#pragma once

#include "google/protobuf/service.h"
#include "muduo/net/EventLoop.h"
#include "MyrpcApplication.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/InetAddress.h"
#include "google/protobuf/descriptor.h"
#include <functional>
#include <unordered_map>


class RpcProvider {
public:
    //框架提供的外部接口，用于注册rpc服务
    void NotifyService(google::protobuf::Service *service);

    //启动rpc服务节点
    void run();

private:
    muduo::net::EventLoop m_mainLoop;

    // 新连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr &);

    // 新信息回调
    void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);

    // 保存服务信息
    struct ServiceInfo {
        google::protobuf::Service *m_service; // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; // 保存方法对象名和方法指针
    };

    // 发送响应信息
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);


    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
};
