#ifndef CHATMANAGESERVICE_H
#define CHATMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include "models/entity/chatlog/ChatRoom.h"
#include "models/entity/chatlog/ChatUnit.h"
#include "models/repository/ChatRoomJsonRepo.h"
#include "models/repository/ChatJsonRepo.h"
#include "utils/CustomErrorCodes.h"

class ChatManageService : public QObject
{
    Q_OBJECT

private:
    static ChatManageService* instance;
    ChatRoomJsonRepo* chatRoomRepo;
    ChatJsonRepo* chatRepo;

    ChatManageService(QObject *parent = nullptr);
    ChatManageService(const ChatManageService&) = delete;
    ChatManageService& operator=(const ChatManageService&) = delete;

public:
    static ChatManageService* getInstance();
    static void destroyInstance();
    ~ChatManageService() = default;

    // 채팅방 관리 메서드들
    RaErrorCode createChatRoom(const ChatRoom& chatRoom);
    RaErrorCode updateChatRoom(const ChatRoom& chatRoom);
    RaErrorCode deleteChatRoom(id_t chatRoomId);
    ChatRoom* getChatRoomById(id_t chatRoomId);
    QVector<ChatRoom> getChatRoomsByUserId(id_t userId);
    QVector<ChatRoom> getAllChatRooms();

    // 채팅방 사용자 관리
    RaErrorCode addUserToChatRoom(id_t chatRoomId, id_t userId);
    RaErrorCode removeUserFromChatRoom(id_t chatRoomId, id_t userId);
    bool isUserInChatRoom(id_t chatRoomId, id_t userId);
    QVector<id_t> getChatRoomUsers(id_t chatRoomId);

    // 채팅 메시지 관리
    RaErrorCode sendMessage(const id_t roomId, const ChatUnit& chatUnit);
    RaErrorCode deleteMessage(id_t chatId);
    const ChatUnit* getMessageById(id_t chatId);
    QVector<ChatUnit> getMessagesByChatRoom(id_t chatRoomId);
    QVector<ChatUnit> getMessagesByUser(id_t userId);
    QVector<ChatUnit> getMessagesInTimeRange(id_t chatRoomId, const QDateTime& startTime, const QDateTime& endTime);

    // 채팅방 검색
    QVector<ChatRoom> searchChatRoomsByName(const QString& name);
    QVector<ChatUnit> searchMessages(const QString& searchText);

    // 통계 및 정보
    int getMessageCount(id_t chatRoomId);
    int getTotalMessageCount();
    QDateTime getLastMessageTime(id_t chatRoomId);

    // 데이터 관리
    bool loadChatData(const QString& chatRoomFilePath, const QString& chatFilePath);
    bool saveChatData();
    void debugPrint();

signals:
    void chatRoomCreated(id_t chatRoomId);
    void chatRoomUpdated(id_t chatRoomId);
    void chatRoomDeleted(id_t chatRoomId);
    void userJoinedChatRoom(id_t chatRoomId, id_t userId);
    void userLeftChatRoom(id_t chatRoomId, id_t userId);
    void messageSent(id_t chatRoomId, id_t chatId);
    void messageDeleted(id_t chatRoomId, id_t chatId);
};

#endif // CHATMANAGESERVICE_H
