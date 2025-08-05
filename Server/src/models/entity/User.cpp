#include "User.h"

// getter
id_t User::getId() const {
    return userId_;
}

id_t User::getPlantId() const {
    return userId_;
}

QString User::getstrId() const {
    return strId_;
}

QString User::getName() const {
    return name_;
}

QString User::getEmail() const {
    return email_;
}

QString User::getAddress() const {
    return address_;
}

bool User::isConnected() const {
    return boolConnected_;
}

// setter
void User::setId(id_t id){
    userId_ = id;
}

void User::setPlantId(id_t id){
    userId_ = id;
}

RaErrorCode User::connect(){
    boolConnected_ = true;
    return Ra_Success;
}

RaErrorCode User::disconnect(){
    boolConnected_ = false;
    return Ra_Success;
}

//etc
bool User::verifystrId(QString inputstrId){
    return (strId_.compare(inputstrId))?false:true;
}

bool User::verifyPassword(QString inputPass){
    return (password_.compare(inputPass))?false:true;
}

bool User::verifyLevel(PermissionLevel inputLevel){
    return (inputLevel == level_)?true:false;
}

// json
QJsonObject User::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("userId",userId_);
    jsonObject.insert("plantId",plantId_);
    jsonObject.insert("strId",strId_);
    jsonObject.insert("password",password_);
    jsonObject.insert("name",name_);
    jsonObject.insert("email",email_);
    jsonObject.insert("address",address_);
    jsonObject.insert("level",level_);

    return jsonObject;
}

RaErrorCode User::fromJson(const QJsonObject& inputJson){
    userId_ = inputJson.value("userId").toInteger();
    plantId_ = inputJson.value("plantId").toInteger();
    strId_ = inputJson.value("strId").toString();
    password_ = inputJson.value("password").toString();
    name_ = inputJson.value("name").toString();
    email_ = inputJson.value("email").toString();
    address_ = inputJson.value("address").toString();
    level_ = (PermissionLevel)inputJson.value("level").toInt();

    return Ra_Success;
}
