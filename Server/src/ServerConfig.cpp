#include "ServerConfig.h"

ServerConfig* ServerConfig::instance = nullptr;

ServerConfig::ServerConfig(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ServerConfig* ServerConfig::getInstance() {
    if (instance == nullptr) {
        instance = new ServerConfig();
    }
    return instance;
}

void ServerConfig::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

