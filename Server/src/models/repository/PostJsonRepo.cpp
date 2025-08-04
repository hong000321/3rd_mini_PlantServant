#include "PostJsonRepo.h"

PostJsonRepo* PostJsonRepo::instance_ = nullptr;

PostJsonRepo::PostJsonRepo() : JsonRepo<Post>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

PostJsonRepo* PostJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new PostJsonRepo();
    }
    return instance_;
}

void PostJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
