#ifndef CHATUNIT_H
#define CHATUNIT_H

#include "models/entity/Entity.h"

class ChatUnit : Entity{
private:
    id_t chatId_;
    id_t userId_;
    QDateTime chatTime_;
    QString chatStr_;

public:
    ChatUnit() = default;
    ChatUnit(id_t id, id_t userId, QDateTime chatTime, QString chatStr)
        : chatId_(id), userId_(userId), chatTime_(chatTime), chatStr_(chatStr) {}

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
