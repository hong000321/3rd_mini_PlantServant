#ifndef SENSORDB_H
#define SENSORDB_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonObject>
#include "utils/CustomErrorCodes.h"
#include "models/entity/SensorData.h"

class SensorDB : public QObject
{
    Q_OBJECT

private:
    static SensorDB* instance_;
    QSqlDatabase database_;
    QString dbFilePath_;

    SensorDB(QObject *parent = nullptr);
    SensorDB(const SensorDB&) = delete;
    SensorDB& operator=(const SensorDB&) = delete;


public:
    static SensorDB* getInstance();
    static void destroyInstance();
    ~SensorDB() = default;

    // 기본 기능만
    RaErrorCode addSensorData(id_t plantId, double temperature, int humidity);
    SensorData getLatestSensorDataByPlant(id_t plantId) const;


    bool initializeDatabase(const QString dbFilePath);
    bool createTables();

signals:
    void sensorDataAdded(const SensorData& data);
    void databaseError(const QString& error);
};

#endif // SENSORDB_H
