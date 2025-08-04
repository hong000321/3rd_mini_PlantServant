#include "Chat.h"

Chat::Chat(QObject *parent)
    : QObject{parent}
{}

// getter
id_t Chat::getSenderId() const {
    return senderId_;
}

QString Chat::getSenderName() const {
    return senderName_;
}

QString Chat::getChatStr() const {
    return chatStr_;
}

// setter
bool Chat::setSenderId(const id_t& senderId) {
    if (senderId < 0)
        return false;
    senderId_ = senderId;
    return true;
}

bool Chat::setSenderName(const QString& senderName) {
    if (senderName.isEmpty())
        return false;
    senderName_ = senderName;
    return true;
}

bool Chat::setChatStr(const QString& chatStr) {
    if (chatStr.isEmpty())
        return false;
    chatStr_ = chatStr;
    return true;
}

QJsonObject Chat::toJson() const {
    QJsonObject obj;
    obj["senderId"] = static_cast<qint64>(senderId_);
    obj["senderName"] = senderName_;
    obj["chatStr"] = chatStr_;
    return obj;
}

Chat Chat::fromJson(const QJsonObject& chatObj){
    id_t senderId = chatObj["senderId"].toVariant().toLongLong();
    QString senderName = chatObj["senderName"].toString();
    QString chatStr = chatObj["chatStr"].toString();

    return Chat(senderId, senderName, chatStr);
}
