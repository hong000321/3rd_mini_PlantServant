#include "ChatUnit.h"


// getter
id_t ChatUnit::getId() const {
    return chatId;
}

id_t ChatUnit::getUserId() const {
    return userId;
}

QDateTime ChatUnit::getChatTime() const {
    return chatTime;
}

QString ChatUnit::getChatStr() const {
    return chatStr;
}

// setter X
void ChatUnit::setId(id_t id){
    chatId = id;
}

// json
QJsonObject ChatUnit::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("chatId",chatId);
    jsonObject.insert("userId",userId);
    jsonObject.insert("chatTime",chatTime.toString());
    jsonObject.insert("chatStr",chatStr);

    return jsonObject;
}

RaErrorCode ChatUnit::fromJson(const QJsonObject& inputJson) {
    chatId = inputJson.value("chatId").toInteger();
    userId = inputJson.value("userId").toInteger();
    chatTime.fromString(inputJson.value("chatTime").toString());
    chatStr = inputJson.value("chatStr").toString();

    return Ra_Success;
}
