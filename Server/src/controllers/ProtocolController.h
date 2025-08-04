#ifndef PROTOCOLCONTROLLER_H
#define PROTOCOLCONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QVector>
#include <QMap>

// 서비스 클래스들 include
#include "models/service/UserManageService.h"
#include "models/service/PlantManageService.h"
#include "models/service/PostManageService.h"
#include "models/service/ChatManageService.h"
#include "models/service/loginService.h"

// 전방 선언
class SocketServer;

class ProtocolController : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolController(QObject *parent = nullptr);
    ~ProtocolController();

    // SocketServer 설정
    void setSocketServer(SocketServer *server) { socketServer_ = server; }

    // 초기화 (post 관련 매개변수 추가)
    bool initialize(const QString& userFilePath, const QString& plantFilePath,
                    const QString& postFilePath, const QString& chatRoomFilePath,
                    const QString& chatFilePath);

    // 메인 처리 메서드
    QJsonObject processMessage(const QJsonObject &message);

public slots:
    void onClientMessageReceived(const QString &clientId, const QJsonObject &data);

signals:
    void sendToClient(const QString &clientId, const QJsonObject &response);
    void sendToUser(const QString &userId, const QJsonObject &message);
    void broadcastToClients(const QJsonObject &message);
    void broadcastToUsers(const QStringList &userIds, const QJsonObject &message);
    void broadcastToChatRoom(id_t chatRoomId, const QJsonObject &message);
    void errorOccurred(const QString &error);

private:
    // 서비스 인스턴스들 (PostManageService 추가)
    UserManageService* userService_;
    PlantManageService* plantService_;
    PostManageService* postService_;
    ChatManageService* chatService_;
    LoginService* loginService_;
    SocketServer* socketServer_;

    // 세션 관리
    QMap<QString, QString> clientSessions_; // clientId -> sessionId

    // 메시지 처리 메서드들
    QJsonObject processCommand(const QJsonObject &header, const QJsonObject &body, const QString &clientId);
    QJsonObject createResponse(const QString &correlationId, const QString &status,
                               int statusCode, const QString &message,
                               const QJsonObject &data = QJsonObject());
    QJsonObject createErrorResponse(const QString &correlationId, const QString &errorCode,
                                    const QString &errorMessage);

    // User 관련 처리 (loginService_ 사용)
    QJsonObject handleUserLogin(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handleUserLogout(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handleUserRegister(const QJsonObject &parameters);
    QJsonObject handleUserGet(const QJsonObject &parameters);
    QJsonObject handleUserUpdate(const QJsonObject &parameters);

    // Plant 관련 처리 (PlantManageService 사용)
    QJsonObject handlePlantList(const QJsonObject &parameters);
    QJsonObject handlePlantGet(const QJsonObject &parameters);
    QJsonObject handlePlantCreate(const QJsonObject &parameters);
    QJsonObject handlePlantUpdate(const QJsonObject &parameters);
    QJsonObject handlePlantDelete(const QJsonObject &parameters);
    QJsonObject handlePlantSensorUpdate(const QJsonObject &parameters);

    // Post 관련 처리 (PostManageService 사용)
    QJsonObject handlePostList(const QJsonObject &parameters);
    QJsonObject handlePostGet(const QJsonObject &parameters);
    QJsonObject handlePostCreate(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handlePostUpdate(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handlePostDelete(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handlePostSearch(const QJsonObject &parameters);

    // Chat 관련 처리 (ChatManageService 사용)
    QJsonObject handleChatRoomCreate(const QJsonObject &parameters);
    QJsonObject handleChatRoomJoin(const QJsonObject &parameters);
    QJsonObject handleChatRoomLeave(const QJsonObject &parameters);
    QJsonObject handleChatSend(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handleChatHistory(const QJsonObject &parameters);
    QJsonObject handleChatRoomList(const QJsonObject &parameters);

    // 채팅 브로드캐스트 처리
    void broadcastChatMessage(id_t chatRoomId, const ChatUnit* chatUnit);

    // 유틸리티 메서드들
    bool validateMessage(const QJsonObject &message);
    bool checkAuthentication(const QString &clientId);
    bool checkPermission(const QString &clientId, const QString &action, const QString &target);
    QString generateId();
    id_t getCurrentUserId(const QString &clientId);

    // 데이터 변환 유틸리티 (Post 관련 추가)
    QJsonObject userToJson(const User* user);
    QJsonObject plantToJson(const Plant* plant);
    QJsonObject postToJson(const Post* post);
    QJsonObject chatRoomToJson(const ChatRoom* chatRoom);
    QJsonObject chatUnitToJson(const ChatUnit* chatUnit);
    QJsonArray vectorToJsonArray(const QVector<Plant>& plants);
    QJsonArray vectorToJsonArray(const QVector<Post>& posts);
    QJsonArray vectorToJsonArray(const QVector<ChatRoom>& products);
};

#endif // PROTOCOLCONTROLLER_H
