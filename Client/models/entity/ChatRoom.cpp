#include "ChatRoom.h"

// getter
id_t ChatRoom::getId() const {
    return chatRoomId_;
}

QString ChatRoom::getRoomName() const {
    return chatRoomName_;
}

QVector<id_t> ChatRoom::getUserIds() const {
    return userIds_;
}

QVector<id_t> ChatRoom::getChatIds() const {
    return chatIds_;
}

// setter
void ChatRoom::setId(id_t id){
    chatRoomId_ = id;
}

RaErrorCode ChatRoom::setUserIds(QVector<id_t> inputUserIds){
    userIds_ = inputUserIds;
    return Ra_Success;
}

RaErrorCode ChatRoom::setChatIds(QVector<id_t> inputChatIds){
    chatIds_ = inputChatIds;
    return Ra_Success;
}


RaErrorCode ChatRoom::setChatRoomName(QString newName){
    chatRoomName_ = newName;
    return Ra_Success;
}

RaErrorCode ChatRoom::addChatId(id_t id){
    chatIds_.append(id);
    return Ra_Success;
}

// json
QJsonObject ChatRoom::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("chatRoomId",chatRoomId_);
    jsonObject.insert("chatRoomName",chatRoomName_);

    // QJsonObject jsonUserIds;
    // for(int i=0; i<userIds.size(); i++){
    //     QString tmpStr = QString("userId%1").arg(i);
    //     jsonUserIds.insert(tmpStr, userIds[i]);
    // }
    // jsonObject.insert("userIds", jsonUserIds);

    QJsonObject jsonChatIds;
    for(int i=0; i<chatIds_.size(); i++){
        QString tmpStr = QString("chatId%1").arg(i);
        jsonChatIds.insert(tmpStr, chatIds_[i]);
    }
    jsonObject.insert("chatIds", jsonChatIds);

    return jsonObject;
}

RaErrorCode ChatRoom::fromJson(const QJsonObject& inputJson){
    chatRoomId_ = inputJson.value("chatRoomId").toInteger();
    chatRoomName_ = inputJson.value("chatRoomName").toString();

    // QJsonObject jsonUserIds = inputJson.value("userIds").toObject();
    // qint32 size = jsonUserIds.size();
    // for(int i=0; i<size; i++){
    //     QString tmpStr = QString("userId%1").arg(i);
    //     userIds.append(jsonUserIds.value(tmpStr).toInteger());
    // }

    QJsonObject jsonChatIds = inputJson.value("chatIds").toObject();
    qint32 size = jsonChatIds.size();
    for(int i=0; i<size; i++){
        QString tmpStr = QString("chatId%1").arg(i);
        chatIds_.append(jsonChatIds.value(tmpStr).toInteger());
    }

    return Ra_Success;
}
