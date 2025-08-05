#include "SensorDB.h"
#include "models/service/PlantManageService.h"
#include <QDebug>
#include <QDir>
#include <QSqlRecord>
#include <QVariant>

SensorDB* SensorDB::instance_ = nullptr;

SensorDB::SensorDB(QObject *parent)
    : QObject(parent)
{
}

SensorDB* SensorDB::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new SensorDB();
    }
    return instance_;
}

void SensorDB::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

bool SensorDB::initializeDatabase()
{
    // 데이터 디렉토리 생성 확인
    QFileInfo fileInfo(dbFilePath_);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create data directory:" << dir.absolutePath();
            return false;
        }
    }

    // SQLite 데이터베이스 연결
    database_ = QSqlDatabase::addDatabase("QSQLITE", "SensorDB");
    database_.setDatabaseName(dbFilePath_);

    if (!database_.open()) {
        qDebug() << "Failed to open database:" << database_.lastError().text();
        emit databaseError("Failed to open database: " + database_.lastError().text());
        return false;
    }

    qDebug() << "SQLite database opened:" << dbFilePath_;
    return createTables();
}

bool SensorDB::createTables()
{
    QSqlQuery query(database_);

    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS sensor_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            plant_id INTEGER NOT NULL,
            temperature REAL NOT NULL,
            humidity INTEGER NOT NULL,
            timestamp TEXT NOT NULL
        )
    )";

    if (!query.exec(createTableSQL)) {
        qDebug() << "Failed to create sensor_data table:" << query.lastError().text();
        emit databaseError("Failed to create table: " + query.lastError().text());
        return false;
    }

    // 인덱스 생성 (성능 향상)
    QString createIndexSQL = R"(
        CREATE INDEX IF NOT EXISTS idx_plant_timestamp
        ON sensor_data(plant_id, timestamp DESC)
    )";

    if (!query.exec(createIndexSQL)) {
        qDebug() << "Failed to create index:" << query.lastError().text();
        // 인덱스 생성 실패는 치명적이지 않음
    }

    qDebug() << "Sensor data table created successfully";
    return true;
}

RaErrorCode SensorDB::addSensorData(id_t plantId, double temperature, int humidity)
{
    if (!database_.isOpen()) {
        qDebug() << "Database is not open";
        emit databaseError("Database is not open");
        return Ra_Domain_Unkown_Error;
    }

    try {
        // plant에 최신 센서 데이터 업데이트
        PlantManageService *plantService = PlantManageService::getInstance();
        plantService->updateTemperature(plantId, temperature);
        plantService->updateHumidity(plantId, humidity);

        // sql에 저장
        QSqlQuery query(database_);

        QString insertSQL = R"(
            INSERT INTO sensor_data (plant_id, temperature, humidity, timestamp)
            VALUES (?, ?, ?, ?)
        )";

        query.prepare(insertSQL);
        query.addBindValue(plantId);
        query.addBindValue(temperature);
        query.addBindValue(humidity);
        query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

        if (!query.exec()) {
            qDebug() << "Failed to insert sensor data:" << query.lastError().text();
            emit databaseError("Failed to insert data: " + query.lastError().text());
            return Ra_Domain_Unkown_Error;
        }

        // 시그널 발생용 데이터 생성
        SensorData data;
        data.plantId = plantId;
        data.temperature = temperature;
        data.humidity = humidity;
        data.timestamp = QDateTime::currentDateTime();

        emit sensorDataAdded(data);
        qDebug() << "Sensor data added to DB - Plant:" << plantId
                 << "Temp:" << temperature << "Humidity:" << humidity;

        return Ra_Success;

    } catch (...) {
        qDebug() << "Exception occurred while adding sensor data";
        emit databaseError("Exception occurred while adding sensor data");
        return Ra_Domain_Unkown_Error;
    }
}


SensorData SensorDB::getLatestSensorDataByPlant(id_t plantId) const
{
    SensorData result;

    if (!database_.isOpen()) {
        qDebug() << "Database is not open";
        return result;
    }

    try {
        QSqlQuery query(database_);

        QString selectSQL = R"(
            SELECT plant_id, temperature, humidity, timestamp
            FROM sensor_data
            WHERE plant_id = ?
            ORDER BY timestamp DESC
            LIMIT 1
        )";

        query.prepare(selectSQL);
        query.addBindValue(plantId);

        if (!query.exec()) {
            qDebug() << "Failed to query latest sensor data:" << query.lastError().text();
            return result;
        }

        if (query.next()) {
            result.plantId = query.value("plant_id").toLongLong();
            result.temperature = query.value("temperature").toDouble();
            result.humidity = query.value("humidity").toDouble();
            result.timestamp = QDateTime::fromString(query.value("timestamp").toString(), Qt::ISODate);

            qDebug() << "Found latest sensor data for plant" << plantId
                     << "- Temp:" << result.temperature << "Humidity:" << result.humidity;
        } else {
            qDebug() << "No sensor data found for plant:" << plantId;
        }

    } catch (...) {
        qDebug() << "Exception occurred while querying sensor data";
    }

    return result;
}

