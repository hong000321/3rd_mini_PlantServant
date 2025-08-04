#include "ProductJsonRepo.h"

ProductJsonRepo* ProductJsonRepo::instance_ = nullptr;

ProductJsonRepo::ProductJsonRepo() : JsonRepo<Product>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ProductJsonRepo* ProductJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new ProductJsonRepo();
    }
    return instance_;
}

void ProductJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
