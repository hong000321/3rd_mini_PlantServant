#include "ChatJsonRepo.h"

ChatJsonRepo* ChatJsonRepo::instance_ = nullptr;

ChatJsonRepo::ChatJsonRepo() : JsonRepo<ChatUnit>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ChatJsonRepo* ChatJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new ChatJsonRepo();
    }
    return instance_;
}

void ChatJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
