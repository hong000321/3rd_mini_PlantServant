#include "ChatJsonRepo.h"

ChatJsonRepo* ChatJsonRepo::instance = nullptr;

ChatJsonRepo::ChatJsonRepo() : JsonRepo<ChatUnit>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ChatJsonRepo* ChatJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new ChatJsonRepo();
    }
    return instance;
}

void ChatJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}
