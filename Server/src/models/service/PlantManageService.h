#ifndef PLANTMANAGESERVICE_H
#define PLANTMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "models/entity/Plant.h"
#include "models/repository/PlantJsonRepo.h"
#include "utils/CustomErrorCodes.h"

class PlantManageService : public QObject
{
    Q_OBJECT

private:
    static PlantManageService* instance_;
    PlantJsonRepo* plantRepo_;

    PlantManageService(QObject *parent = nullptr);
    PlantManageService(const PlantManageService&) = delete;
    PlantManageService& operator=(const PlantManageService&) = delete;

public:
    static PlantManageService* getInstance();
    static void destroyInstance();
    ~PlantManageService() = default;

    // 식물 관리 메서드들
    RaErrorCode createPlant(const Plant& plant);
    RaErrorCode updatePlant(const Plant& plant);
    RaErrorCode deletePlant(id_t plantId);
    const Plant* getPlantById(id_t plantId);
    QVector<Plant> getPlantsByUserId(id_t userId);
    QVector<Plant> getAllPlants();

    // 센서 데이터 업데이트
    RaErrorCode updateSensorData(id_t plantId, int humidity, double temperature);
    RaErrorCode updateHumidity(id_t plantId, int humidity);
    RaErrorCode updateTemperature(id_t plantId, double temperature);

    // 식물 정보 관리
    RaErrorCode updateNickname(id_t plantId, const QString& newNickname);

    // 검색 기능
    QVector<Plant> searchPlantsByNickname(const QString& nickname);
    QVector<Plant> getPlantsInHumidityRange(int minHumidity, int maxHumidity);
    QVector<Plant> getPlantsInTemperatureRange(double minTemp, double maxTemp);

    // 데이터 관리
    bool loadPlants(const QString& filePath);
    bool savePlants();
    int getPlantCount();

signals:
    void plantCreated(id_t plantId);
    void plantUpdated(id_t plantId);
    void plantDeleted(id_t plantId);
    void sensorDataUpdated(id_t plantId, int humidity, double temperature);
    void humidityUpdated(id_t plantId, int humidity);
    void temperatureUpdated(id_t plantId, double temperature);
};

#endif // PLANTMANAGESERVICE_H
