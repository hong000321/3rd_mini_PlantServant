#include "Plant.h"

// getter
id_t Plant::getId() const {
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
void Plant::setId(id_t id) {
    plantId_ = id;
}

RaErrorCode Plant::setNickName(const QString& inputName) {
    if (inputName.isEmpty()) {
        return Ra_Domain_Unkown_Error;
    }
    nickname_ = inputName;
    return Ra_Success;
}

RaErrorCode Plant::setHumidity(const int& humidity_data) {
    humidity_ = humidity_data;
    return Ra_Success;
}

RaErrorCode Plant::setTemperature(const double& temperature_data) {
    temperature_ = temperature_data;
    return Ra_Success;
}

RaErrorCode Plant::setUserId(const id_t& userId) {
    if (userId < 0) {
        return Ra_Domain_Unkown_Error;
    }
    userId_ = userId;
    return Ra_Success;
}

// json
QJsonObject Plant::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("plantId", plantId_);
    jsonObject.insert("nickname", nickname_);
    jsonObject.insert("humidity", humidity_);
    jsonObject.insert("temperature", temperature_);
    jsonObject.insert("userId", userId_);

    return jsonObject;
}

RaErrorCode Plant::fromJson(const QJsonObject& inputJson) {
    plantId_ = inputJson.value("plantId").toInteger();
    nickname_ = inputJson.value("nickname").toString();
    humidity_ = inputJson.value("humidity").toInt();
    temperature_ = inputJson.value("temperature").toDouble();
    userId_ = inputJson.value("userId").toInteger();

    return Ra_Success;
}
