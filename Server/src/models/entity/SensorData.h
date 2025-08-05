#ifndef SENSORDATA_H
#define SENSORDATA_H
#include "Entity.h"
class SensorData
{
public:
    SensorData();

    id_t plantId;
    double temperature;
    int humidity;
    QDateTime timestamp;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["plantId"] = plantId;
        obj["temperature"] = temperature;
        obj["humidity"] = humidity;
        obj["timestamp"] = timestamp.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& obj) {
        plantId = obj["plantId"].toInteger();
        temperature = obj["temperature"].toDouble();
        humidity = obj["humidity"].toInt();
        timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
    }
};

#endif // SENSORDATA_H
