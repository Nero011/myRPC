//
// Created by root on 10/13/23.
// 服务调用端
//
#include <iostream>
#include "MyrpcApplication.h"
#include "MyRpcChannel.h"
#include "user.pb.h"
#include "MyrpcController.h"
#include <string>

int main(int argc, char **argv) {
    MyrpcApplication::init(argc, argv);


    fixbug::UserServiceRpc_Stub stub(new MyRpcChannel());

    fixbug::LoginRequest request;
    fixbug::LoginResponse response;

    request.set_name("zhangsan");
    request.set_pwd("123456");
    MyrpcController controller;
    stub.Login(&controller, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果
    if (controller.Failed()) {
        std::cout << "rpc login response error : " << std::endl;
        std::cout << controller.ErrorText();
    }
    std::cout << "rpc login response success:" << response.sucess() << std::endl;



    // 演示调用远程发布的rpc方法Register
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("myrpc");
    req.set_pwd("666666");
    fixbug::RegisterResponse rsp;

    // 以同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr);

    // 一次rpc调用完成，读调用的结果
    if (0 == rsp.result().errcode()) {
        std::cout << "rpc register response success:" << rsp.sucess() << std::endl;
    } else {
        std::cout << "rpc register response error : " << rsp.result().errmsg() << std::endl;
    }

    return 0;


}