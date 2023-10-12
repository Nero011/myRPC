//
// Created by root on 10/10/23.
//

#include "MyrpcConfig.h"
#include <iostream>

// 去除字符串前后空格
void Trim(std::string &s) {
    int idx = s.find_first_not_of(' ');
    if (idx != -1) {
        s = s.substr(idx, s.size() - idx);
    }
    idx = s.find_last_not_of(' ');
    if (idx != -1) {
        s = s.substr(0, idx + 1);
    }
}

void MyrpcConfig::LoadConfigFile(const char *config_file) {
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cout << config_file << "is not exist !" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!feof(pf)) {
        char buf[512] = {};
        fgets(buf, 512, pf);
        std::string read_buf = buf;
        Trim(read_buf);

        if (read_buf[0] == '#' || read_buf.empty()) {
            continue;
        }
        int idx = read_buf.find('=');
        if (idx == -1) {
            continue;
        }
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        // rpcserverip=127.0.0.1\n
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        m_configMap.insert({key, value});
    }
    fclose(pf);
    for (auto i: m_configMap) {
        std::cout << "Load " << i.first << " " << i.second << std::endl;
    }
}

std::string MyrpcConfig::Load(std::string key) {
    auto it = m_configMap.find(key);
    if (it != m_configMap.end()) {
        return it->second;
    }
    return "";

}

