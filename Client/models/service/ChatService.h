#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include "controllers/ClientSocket.h"

#define id_t qint64

class ChatService : public QObject {
    Q_OBJECT
public:
    explicit ChatService(ClientSocket* socket, QObject* parent = nullptr);

    void requestChatRoomList();
    void joinChatRoom(id_t roomId, id_t userId);
    void leaveChatRoom(id_t roomId, id_t userId);
    void sendMessage(id_t roomId, const QString& message);
    void requestChatHistory(id_t roomId);

signals:
    void chatRoomListReceived(const QJsonArray&);
    void chatRoomJoined(const QJsonObject&);
    void chatRoomLeft(bool success);
    void chatMessageSent(const QJsonObject&);
    void chatMessageReceived(const QJsonObject&);
    void chatHistoryReceived(const QJsonArray&);

private slots:
    void onChatRoomListReceived(const QJsonArray&);
    void onChatRoomJoined(const QJsonObject&);
    void onChatRoomLeft(bool success);
    void onChatMessageSent(const QJsonObject&);
    void onChatMessageReceived(const QJsonObject&);
    void onChatHistoryReceived(const QJsonArray&);

private:
    ClientSocket* socket_;
};

#endif // CHATSERVICE_H
