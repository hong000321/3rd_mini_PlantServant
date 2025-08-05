// ChatService.cpp
#include "ChatService.h"
#include <QDebug>

ChatService::ChatService(ClientSocket* socket, QObject* parent)
    : QObject(parent), socket_(socket)
{
    connect(socket_, &ClientSocket::chatRoomListReceived, this, &ChatService::onChatRoomListReceived);
    connect(socket_, &ClientSocket::chatRoomJoined, this, &ChatService::onChatRoomJoined);
    connect(socket_, &ClientSocket::chatRoomLeft, this, &ChatService::onChatRoomLeft);
    connect(socket_, &ClientSocket::chatMessageSent, this, &ChatService::onChatMessageSent);
    connect(socket_, &ClientSocket::chatMessageReceived, this, &ChatService::onChatMessageReceived);
    connect(socket_, &ClientSocket::chatHistoryReceived, this, &ChatService::onChatHistoryReceived);
}

void ChatService::requestChatRoomList() {
    socket_->requestChatRoomList();
}

void ChatService::joinChatRoom(id_t roomId, id_t userId) {
    socket_->joinChatRoom(roomId, userId);
}

void ChatService::leaveChatRoom(id_t roomId, id_t userId) {
    socket_->leaveChatRoom(roomId, userId);
}

void ChatService::sendMessage(id_t roomId, const QString& message) {
    socket_->sendChatMessage(roomId, message);
}

void ChatService::requestChatHistory(id_t roomId) {
    socket_->requestChatHistory(roomId);
}

void ChatService::onChatRoomListReceived(const QJsonArray& rooms) {
    emit chatRoomListReceived(rooms);
}

void ChatService::onChatRoomJoined(const QJsonObject& room) {
    emit chatRoomJoined(room);
}

void ChatService::onChatRoomLeft(bool success) {
    emit chatRoomLeft(success);
}

void ChatService::onChatMessageSent(const QJsonObject& msg) {
    emit chatMessageSent(msg);
}

void ChatService::onChatMessageReceived(const QJsonObject& msg) {
    emit chatMessageReceived(msg);
}

void ChatService::onChatHistoryReceived(const QJsonArray& history) {
    emit chatHistoryReceived(history);
}
