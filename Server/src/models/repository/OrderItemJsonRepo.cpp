#include "OrderItemJsonRepo.h"

OrderItemJsonRepo* OrderItemJsonRepo::instance_ = nullptr;

OrderItemJsonRepo::OrderItemJsonRepo() : JsonRepo<OrderItem>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

OrderItemJsonRepo* OrderItemJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new OrderItemJsonRepo();
    }
    return instance_;
}

void OrderItemJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
