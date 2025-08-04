#include "OrderJsonRepo.h"

OrderJsonRepo* OrderJsonRepo::instance = nullptr;

OrderJsonRepo::OrderJsonRepo() : JsonRepo<Order>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

OrderJsonRepo* OrderJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new OrderJsonRepo();
    }
    return instance;
}

void OrderJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}
