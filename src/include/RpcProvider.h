//
// Created by root on 10/9/23.
// 用于发布rpc网络服务的对象类
//

#pragma once
#include "google/protobuf/service.h"

class RpcProvider{
public:
    //框架提供的外部接口，用于注册rpc服务
    void NotifyService(google::protobuf::Service* service);
    //启动rpc服务节点
    void run();
};
