#include "OrderJsonRepo.h"

OrderJsonRepo* OrderJsonRepo::instance_ = nullptr;

OrderJsonRepo::OrderJsonRepo() : JsonRepo<Order>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

OrderJsonRepo* OrderJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new OrderJsonRepo();
    }
    return instance_;
}

void OrderJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
