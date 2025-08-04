#include "ProductJsonRepo.h"

ProductJsonRepo* ProductJsonRepo::instance = nullptr;

ProductJsonRepo::ProductJsonRepo() : JsonRepo<Product>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ProductJsonRepo* ProductJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new ProductJsonRepo();
    }
    return instance;
}

void ProductJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}
