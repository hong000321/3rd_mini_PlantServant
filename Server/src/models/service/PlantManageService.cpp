#include "PlantManageService.h"
#include <QDebug>

PlantManageService* PlantManageService::instance_ = nullptr;

PlantManageService::PlantManageService(QObject *parent)
    : QObject(parent)
{
    plantRepo_ = PlantJsonRepo::getInstance();
}

PlantManageService* PlantManageService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new PlantManageService();
    }
    return instance_;
}

void PlantManageService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode PlantManageService::createPlant(const Plant& plant)
{
    id_t plantId = plantRepo_->insert(plant);
    if (plantId >= 0) {
        emit plantCreated(plantId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PlantManageService::updatePlant(const Plant& plant)
{
    const Plant* existingPlant = plantRepo_->getObjPtrById(plant.getId());
    if (existingPlant == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (plantRepo_->update(plant)) {
        emit plantUpdated(plant.getId());
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PlantManageService::deletePlant(id_t plantId)
{
    if (plantRepo_->removeById(plantId)) {
        emit plantDeleted(plantId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const Plant* PlantManageService::getPlantById(id_t plantId)
{
    return plantRepo_->getObjPtrById(plantId);
}

QVector<Plant> PlantManageService::getPlantsByUserId(id_t userId)
{
    QVector<Plant> result;
    QVector<Plant> allPlants = plantRepo_->getAllObjects();

    for (const Plant& plant : allPlants) {
        if (plant.getUserId() == userId) {
            result.append(plant);
        }
    }

    return result;
}

QVector<Plant> PlantManageService::getAllPlants()
{
    return plantRepo_->getAllObjects();
}

RaErrorCode PlantManageService::updateSensorData(id_t plantId, int humidity, double temperature)
{
    const Plant* plant = plantRepo_->getObjPtrById(plantId);
    if (plant == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Plant updatedPlant = *plant;
    RaErrorCode humidityResult = updatedPlant.setHumidity(humidity);
    RaErrorCode tempResult = updatedPlant.setTemperature(temperature);

    if (humidityResult == Ra_Success && tempResult == Ra_Success) {
        if (plantRepo_->update(updatedPlant)) {
            emit sensorDataUpdated(plantId, humidity, temperature);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PlantManageService::updateHumidity(id_t plantId, int humidity)
{
    const Plant* plant = plantRepo_->getObjPtrById(plantId);
    if (plant == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Plant updatedPlant = *plant;
    RaErrorCode result = updatedPlant.setHumidity(humidity);
    if (result == Ra_Success) {
        if (plantRepo_->update(updatedPlant)) {
            emit humidityUpdated(plantId, humidity);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PlantManageService::updateTemperature(id_t plantId, double temperature)
{
    const Plant* plant = plantRepo_->getObjPtrById(plantId);
    if (plant == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Plant updatedPlant = *plant;
    RaErrorCode result = updatedPlant.setTemperature(temperature);
    if (result == Ra_Success) {
        if (plantRepo_->update(updatedPlant)) {
            emit temperatureUpdated(plantId, temperature);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode PlantManageService::updateNickname(id_t plantId, const QString& newNickname)
{
    const Plant* plant = plantRepo_->getObjPtrById(plantId);
    if (plant == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Plant updatedPlant = *plant;
    RaErrorCode result = updatedPlant.setNickName(newNickname);
    if (result == Ra_Success) {
        if (plantRepo_->update(updatedPlant)) {
            emit plantUpdated(plantId);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

QVector<Plant> PlantManageService::searchPlantsByNickname(const QString& nickname)
{
    QVector<Plant> result;
    QVector<Plant> allPlants = plantRepo_->getAllObjects();

    for (const Plant& plant : allPlants) {
        if (plant.getNickname().contains(nickname, Qt::CaseInsensitive)) {
            result.append(plant);
        }
    }

    return result;
}

QVector<Plant> PlantManageService::getPlantsInHumidityRange(int minHumidity, int maxHumidity)
{
    QVector<Plant> result;
    QVector<Plant> allPlants = plantRepo_->getAllObjects();

    for (const Plant& plant : allPlants) {
        if (plant.getHumidity() >= minHumidity && plant.getHumidity() <= maxHumidity) {
            result.append(plant);
        }
    }

    return result;
}

QVector<Plant> PlantManageService::getPlantsInTemperatureRange(double minTemp, double maxTemp)
{
    QVector<Plant> result;
    QVector<Plant> allPlants = plantRepo_->getAllObjects();

    for (const Plant& plant : allPlants) {
        if (plant.getTemperature() >= minTemp && plant.getTemperature() <= maxTemp) {
            result.append(plant);
        }
    }

    return result;
}

bool PlantManageService::loadPlants(const QString& filePath)
{
    return plantRepo_->loadDataFromFile(filePath);
}

bool PlantManageService::savePlants()
{
    return plantRepo_->saveToFile();
}

int PlantManageService::getPlantCount()
{
    return plantRepo_->getSize();
}
