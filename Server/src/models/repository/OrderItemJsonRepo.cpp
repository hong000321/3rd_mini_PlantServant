#include "OrderItemJsonRepo.h"

OrderItemJsonRepo* OrderItemJsonRepo::instance = nullptr;

OrderItemJsonRepo::OrderItemJsonRepo() : JsonRepo<OrderItem>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

OrderItemJsonRepo* OrderItemJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new OrderItemJsonRepo();
    }
    return instance;
}

void OrderItemJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}
