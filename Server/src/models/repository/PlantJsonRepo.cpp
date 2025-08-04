#include "PlantJsonRepo.h"

PlantJsonRepo* PlantJsonRepo::instance_ = nullptr;

PlantJsonRepo::PlantJsonRepo() : JsonRepo<Plant>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

PlantJsonRepo* PlantJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new PlantJsonRepo();
    }
    return instance_;
}

void PlantJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}
