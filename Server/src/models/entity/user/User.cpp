#include "User.h"

// getter
id_t User::getId() const {
    return userId;
}

QString User::getstrId() const {
    return strId;
}

QString User::getName() const {
    return name;
}

QString User::getEmail() const {
    return email;
}

QString User::getAddress() const {
    return address;
}

bool User::isConnected() const {
    return boolConnected;
}

// setter
void User::setId(id_t id){
    userId = id;
}

RaErrorCode User::connect(){
    boolConnected = true;
    return Ra_Success;
}

RaErrorCode User::disconnect(){
    boolConnected = false;
    return Ra_Success;
}

//etc
bool User::verifystrId(QString inputstrId){
    return (strId.compare(inputstrId))?false:true;
}

bool User::verifyPassword(QString inputPass){
    return (password.compare(inputPass))?false:true;
}

bool User::verifyLevel(PermissionLevel inputLevel){
    return (inputLevel == level)?true:false;
}

// json
QJsonObject User::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("userId",userId);
    jsonObject.insert("strId",strId);
    jsonObject.insert("password",password);
    jsonObject.insert("name",name);
    jsonObject.insert("email",email);
    jsonObject.insert("address",address);
    jsonObject.insert("level",level);

    return jsonObject;
}

RaErrorCode User::fromJson(const QJsonObject& inputJson){
    userId = inputJson.value("userId").toInteger();
    strId = inputJson.value("strId").toString();
    password = inputJson.value("password").toString();
    name = inputJson.value("name").toString();
    email = inputJson.value("email").toString();
    address = inputJson.value("address").toString();
    level = (PermissionLevel)inputJson.value("level").toInt();

    return Ra_Success;
}
