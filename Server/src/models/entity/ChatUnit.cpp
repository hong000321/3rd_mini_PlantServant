#include "ChatUnit.h"


// getter
id_t ChatUnit::getId() const {
    return chatId_;
}

id_t ChatUnit::getUserId() const {
    return userId_;
}

QDateTime ChatUnit::getChatTime() const {
    return chatTime_;
}

QString ChatUnit::getChatStr() const {
    return chatStr_;
}

// setter X
void ChatUnit::setId(id_t id){
    chatId_ = id;
}

// json
QJsonObject ChatUnit::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("chatId",chatId_);
    jsonObject.insert("userId",userId_);
    jsonObject.insert("chatTime",chatTime_.toString());
    jsonObject.insert("chatStr",chatStr_);

    return jsonObject;
}

RaErrorCode ChatUnit::fromJson(const QJsonObject& inputJson) {
    chatId_ = inputJson.value("chatId").toInteger();
    userId_ = inputJson.value("userId").toInteger();
    chatTime_.fromString(inputJson.value("chatTime").toString());
    chatStr_ = inputJson.value("chatStr").toString();

    return Ra_Success;
}
