//
// Created by root on 10/10/23.
//

#pragma once

#include <unordered_map>
#include <string>

class MyrpcConfig {
public:
    // 负责解析加载配置项文件
    void LoadConfigFile(const char* config_file);
    // 查询配置项信息
    std::string Load(std::string key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
};