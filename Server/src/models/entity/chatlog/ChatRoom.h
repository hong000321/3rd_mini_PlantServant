#ifndef CHATROOM_H
#define CHATROOM_H

#include "models/entity/Entity.h"

class ChatRoom : Entity{
private:
    id_t chatRoomId = -1;
    QString chatRoomName;
    QVector<id_t> userIds;
    QVector<id_t> chatIds;
public:
    ChatRoom() = default;
    ChatRoom(QString chatRoomName, QVector<id_t> userIds, QVector<id_t> chatIds)
        : chatRoomName(chatRoomName), userIds(userIds), chatIds(chatIds) {}

    // getter
    id_t getId() const  override;
    QString getRoomName() const ;
    QVector<id_t> getUserIds() const ;
    QVector<id_t> getChatIds() const ;

    // setter
    void setId(id_t id) override;
    RaErrorCode setUserIds(QVector<id_t> inputUserIds);
    RaErrorCode setChatIds(QVector<id_t> inputChatIds);
    RaErrorCode setChatRoomName(QString newName);
    RaErrorCode addChatId(id_t);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;

};

#endif // CHATROOM_H
