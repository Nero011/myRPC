//
// Created by root on 10/13/23.
//

#pragma once
#include "user.pb.h"
#include "RpcHeader.pb.h"
#include "google/protobuf/service.h"

class MyRpcChannel : public google::protobuf::RpcChannel {
public:
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done);
};
