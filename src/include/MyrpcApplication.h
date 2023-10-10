//
// Created by root on 10/9/23.
// myrpc框架的初始化
//

#pragma once
#include <MyrpcConfig.h>

class MyrpcApplication {
public:
    static void init(int argc, char **argv);
    static MyrpcApplication& getInstance();

private:
    MyrpcApplication();
    MyrpcApplication(const MyrpcApplication &) = delete;

    static MyrpcConfig m_config;
};

