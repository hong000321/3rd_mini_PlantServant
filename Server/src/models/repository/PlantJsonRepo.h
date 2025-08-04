#ifndef PLANTJSONREPO_H
#define PLANTJSONREPO_H
#include "JsonRepo.h"
#include "src/models/Entity/Plant.h"

class PlantJsonRepo : public JsonRepo<Plant> {
private:
    static PlantJsonRepo* instance_;  // 싱글톤 인스턴스
    PlantJsonRepo();
    PlantJsonRepo(const PlantJsonRepo&) = delete;
    PlantJsonRepo& operator=(const PlantJsonRepo&) = delete;
    PlantJsonRepo(PlantJsonRepo&&) = delete;
    PlantJsonRepo& operator=(PlantJsonRepo&&) = delete;

public:
    static PlantJsonRepo* getInstance();
    ~PlantJsonRepo() = default;
    static void destroyInstance();
};

#endif // PLANTJSONREPO_H
