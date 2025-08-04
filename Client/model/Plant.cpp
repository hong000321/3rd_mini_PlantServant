#include "Plant.h"

Plant::Plant(QObject *parent)
    : QObject{parent}
{}

// Getter 구현
id_t Plant::getPlantId() const {
    return plantId_;
}

QString Plant::getNickname() const {
    return nickname_;
}

int Plant::getHumidity() const {
    return humidity_;
}

double Plant::getTemperature() const {
    return temperature_;
}

id_t Plant::getUserId() const {
    return userId_;
}

// setter
bool Plant::setPlantId(id_t id) {
    if (id < 0) return false;
    plantId_ = id;
    return true;
}

bool Plant::setNickName(const QString& inputName) {
    if (inputName.isEmpty()) return false;
    nickname_ = inputName;
    return true;
}

bool Plant::setHumidity(const int& humidity_data) {
    humidity_ = humidity_data;
    return true;
}

bool Plant::setTemperature(const double& temperature_data) {
    temperature_ = temperature_data;
    return true;
}

bool Plant::setUserId(const id_t& userId) {
    if (userId < 0) return false;
    userId_ = userId;
    return true;
}

// toJson
QJsonObject Plant::toJson() const {
    QJsonObject obj;
    obj["plantId"] = static_cast<qint64>(plantId_);
    obj["nickname"] = nickname_;
    obj["humidity"] = humidity_;
    obj["temperature"] = temperature_;
    obj["userId"] = static_cast<qint64>(userId_);
    return obj;
}

// fromJson
Plant Plant::fromJson(const QJsonObject& plantObj) {
    id_t id = plantObj["plantId"].toVariant().toLongLong();
    QString name = plantObj["nickname"].toString();
    int humidity = plantObj["humidity"].toInt();
    double temp = plantObj["temperature"].toDouble();
    id_t userId = plantObj["userId"].toVariant().toLongLong();

    return Plant(id, name, humidity, temp, userId);
}
