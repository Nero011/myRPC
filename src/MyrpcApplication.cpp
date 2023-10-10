//
// Created by root on 10/9/23.
//
#include <iostream>
#include <unistd.h>
#include "include/MyrpcApplication.h"

void ShowArgHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;

}

MyrpcConfig MyrpcApplication::m_config;

MyrpcApplication::MyrpcApplication() {}

void MyrpcApplication::init(int argc, char **argv) {
    if (argc < 2) {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowArgHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    // 加载配置文件
    m_config.LoadConfigFile(config_file.c_str());
}

MyrpcApplication &MyrpcApplication::getInstance() {
    static MyrpcApplication app;
    return app;
}