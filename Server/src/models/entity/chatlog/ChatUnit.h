#ifndef CHATUNIT_H
#define CHATUNIT_H

#include "models/entity/Entity.h"

class ChatUnit : Entity{
private:
    id_t chatId;
    id_t userId;
    QDateTime chatTime;
    QString chatStr;

public:
    ChatUnit() = default;
    ChatUnit(id_t id, id_t userId, QDateTime chatTime, QString chatStr)
        : chatId(id), userId(userId), chatTime(chatTime), chatStr(chatStr) {}

    // getter
    id_t getId() const  override;
    id_t getUserId() const ;
    QDateTime getChatTime() const ;
    QString getChatStr() const ;

    // setter X
    void setId(id_t id) override;

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;


};
#endif // CHATUNIT_H
