#include "ChatManageService.h"
#include <QDebug>

ChatManageService* ChatManageService::instance_ = nullptr;

ChatManageService::ChatManageService(QObject *parent)
    : QObject(parent)
{
    chatRoomRepo_ = ChatRoomJsonRepo::getInstance();
    chatRepo_ = ChatJsonRepo::getInstance();
}

ChatManageService* ChatManageService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new ChatManageService();
    }
    return instance_;
}

void ChatManageService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode ChatManageService::createChatRoom(const ChatRoom& chatRoom)
{
    id_t roomId = chatRoomRepo_->insert(chatRoom);
    if (roomId>=0) {
        emit chatRoomCreated(roomId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ChatManageService::updateChatRoom(const ChatRoom& chatRoom)
{
    const ChatRoom* existingRoom = chatRoomRepo_->getObjPtrById(chatRoom.getId());
    if (existingRoom == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (chatRoomRepo_->update(chatRoom)) {
        emit chatRoomUpdated(chatRoom.getId());
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ChatManageService::deleteChatRoom(id_t chatRoomId)
{
    if (chatRoomRepo_->removeById(chatRoomId)) {
        emit chatRoomDeleted(chatRoomId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

ChatRoom* ChatManageService::getChatRoomById(id_t chatRoomId)
{
    return chatRoomRepo_->getObjPtrById(chatRoomId);
}

QVector<ChatRoom> ChatManageService::getChatRoomsByUserId(id_t userId)
{
    QVector<ChatRoom> result;
    QVector<ChatRoom> allRooms = chatRoomRepo_->getAllObjects();

    for (const ChatRoom& room : allRooms) {
        QVector<id_t> userIds = room.getUserIds();
        if (userIds.contains(userId)) {
            result.append(room);
        }
    }

    return result;
}

QVector<ChatRoom> ChatManageService::getAllChatRooms()
{
    return chatRoomRepo_->getAllObjects();
}

RaErrorCode ChatManageService::addUserToChatRoom(id_t chatRoomId, id_t userId)
{
    const ChatRoom* room = chatRoomRepo_->getObjPtrById(chatRoomId);
    if (room == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    ChatRoom updatedRoom = *room;
    QVector<id_t> userIds = updatedRoom.getUserIds();

    if (!userIds.contains(userId)) {
        userIds.append(userId);
        updatedRoom.setUserIds(userIds);

        if (updateChatRoom(updatedRoom) == Ra_Success) {
            emit userJoinedChatRoom(chatRoomId, userId);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ChatManageService::removeUserFromChatRoom(id_t chatRoomId, id_t userId)
{
    const ChatRoom* room = chatRoomRepo_->getObjPtrById(chatRoomId);
    if (room == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    ChatRoom updatedRoom = *room;
    QVector<id_t> userIds = updatedRoom.getUserIds();

    userIds.removeAll(userId);
    updatedRoom.setUserIds(userIds);

    if (updateChatRoom(updatedRoom) == Ra_Success) {
        emit userLeftChatRoom(chatRoomId, userId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

bool ChatManageService::isUserInChatRoom(id_t chatRoomId, id_t userId)
{
    const ChatRoom* room = chatRoomRepo_->getObjPtrById(chatRoomId);
    if (room == nullptr) {
        return false;
    }

    QVector<id_t> userIds = room->getUserIds();
    return userIds.contains(userId);
}

QVector<id_t> ChatManageService::getChatRoomUsers(id_t chatRoomId)
{
    const ChatRoom* room = chatRoomRepo_->getObjPtrById(chatRoomId);
    return room ? room->getUserIds() : QVector<id_t>();
}

RaErrorCode ChatManageService::sendMessage(const id_t roomId, const ChatUnit& chatUnit)
{
    id_t chatId = chatRepo_->insert(chatUnit);
    if (chatId>=0) {
        ChatRoom *room = chatRoomRepo_->getObjPtrById(roomId);
        if(room != nullptr){
            room->addChatId(chatId);
            chatRoomRepo_->saveToFile();
            emit messageSent(roomId, chatId);
            return Ra_Success;
        }
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ChatManageService::deleteMessage(id_t chatId)
{
    if (chatRepo_->removeById(chatId)) {
        emit messageDeleted(0, chatId); // chatRoomId는 별도로 관리 필요
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const ChatUnit* ChatManageService::getMessageById(id_t chatId)
{
    return chatRepo_->getObjPtrById(chatId);
}

QVector<ChatUnit> ChatManageService::getMessagesByChatRoom(id_t chatRoomId)
{
    const ChatRoom* room = chatRoomRepo_->getObjPtrById(chatRoomId);
    if (room == nullptr) {
        return QVector<ChatUnit>();
    }

    QVector<ChatUnit> result;
    QVector<id_t> chatIds = room->getChatIds();

    for (id_t chatId : chatIds) {
        const ChatUnit* chat = chatRepo_->getObjPtrById(chatId);
        if (chat != nullptr) {
            result.append(*chat);
        }
    }

    return result;
}

QVector<ChatUnit> ChatManageService::getMessagesByUser(id_t userId)
{
    QVector<ChatUnit> result;
    QVector<ChatUnit> allMessages = chatRepo_->getAllObjects();

    for (const ChatUnit& chat : allMessages) {
        if (chat.getUserId() == userId) {
            result.append(chat);
        }
    }

    return result;
}

QVector<ChatUnit> ChatManageService::getMessagesInTimeRange(id_t chatRoomId, const QDateTime& startTime, const QDateTime& endTime)
{
    QVector<ChatUnit> roomMessages = getMessagesByChatRoom(chatRoomId);
    QVector<ChatUnit> result;

    for (const ChatUnit& chat : roomMessages) {
        QDateTime chatTime = chat.getChatTime();
        if (chatTime >= startTime && chatTime <= endTime) {
            result.append(chat);
        }
    }

    return result;
}

QVector<ChatRoom> ChatManageService::searchChatRoomsByName(const QString& name)
{
    QVector<ChatRoom> result;
    QVector<ChatRoom> allRooms = chatRoomRepo_->getAllObjects();

    for (const ChatRoom& room : allRooms) {
        if (room.getRoomName().contains(name, Qt::CaseInsensitive)) {
            result.append(room);
        }
    }

    return result;
}

QVector<ChatUnit> ChatManageService::searchMessages(const QString& searchText)
{
    QVector<ChatUnit> result;
    QVector<ChatUnit> allMessages = chatRepo_->getAllObjects();

    for (const ChatUnit& chat : allMessages) {
        if (chat.getChatStr().contains(searchText, Qt::CaseInsensitive)) {
            result.append(chat);
        }
    }

    return result;
}

int ChatManageService::getMessageCount(id_t chatRoomId)
{
    return getMessagesByChatRoom(chatRoomId).size();
}

int ChatManageService::getTotalMessageCount()
{
    return chatRepo_->getSize();
}

QDateTime ChatManageService::getLastMessageTime(id_t chatRoomId)
{
    QVector<ChatUnit> messages = getMessagesByChatRoom(chatRoomId);

    if (messages.isEmpty()) {
        return QDateTime();
    }

    QDateTime lastTime = messages.first().getChatTime();
    for (const ChatUnit& chat : messages) {
        if (chat.getChatTime() > lastTime) {
            lastTime = chat.getChatTime();
        }
    }

    return lastTime;
}

bool ChatManageService::loadChatData(const QString& chatRoomFilePath, const QString& chatFilePath)
{
    bool roomResult = chatRoomRepo_->loadDataFromFile(chatRoomFilePath);
    bool chatResult = chatRepo_->loadDataFromFile(chatFilePath);
    return roomResult && chatResult;
}

bool ChatManageService::saveChatData()
{
    bool roomResult = chatRoomRepo_->saveToFile();
    bool chatResult = chatRepo_->saveToFile();

    return roomResult && chatResult;
}

void ChatManageService::debugPrint(){
    chatRoomRepo_->debugPrint();
}
