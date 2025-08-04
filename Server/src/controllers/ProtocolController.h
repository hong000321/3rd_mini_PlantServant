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
#include "models/service/ProductManageService.h"
#include "models/service/OrderManageService.h"
#include "models/service/ChatManageService.h"
#include "models/service/LoginService.h"

// 전방 선언
class SocketServer;

class ProtocolController : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolController(QObject *parent = nullptr);
    ~ProtocolController();

    // SocketServer 설정
    void setSocketServer(SocketServer *server) { m_socketServer = server; }

    // 초기화
    bool initialize(const QString& userFilePath, const QString& productFilePath,
                    const QString& orderFilePath, const QString& orderItemFilePath,
                    const QString& chatRoomFilePath, const QString& chatFilePath);

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
    // 서비스 인스턴스들
    UserManageService* userService;
    ProductManageService* productService;
    OrderManageService* orderService;
    ChatManageService* chatService;
    LoginService* loginService;
    SocketServer* m_socketServer;

    // 세션 관리
    QMap<QString, QString> m_clientSessions; // clientId -> sessionId

    // 메시지 처리 메서드들
    QJsonObject processCommand(const QJsonObject &header, const QJsonObject &body, const QString &clientId);
    QJsonObject createResponse(const QString &correlationId, const QString &status,
                               int statusCode, const QString &message,
                               const QJsonObject &data = QJsonObject());
    QJsonObject createErrorResponse(const QString &correlationId, const QString &errorCode,
                                    const QString &errorMessage);

    // User 관련 처리 (LoginService 사용)
    QJsonObject handleUserLogin(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handleUserLogout(const QJsonObject &parameters, const QString &clientId);
    QJsonObject handleUserRegister(const QJsonObject &parameters);
    QJsonObject handleUserGet(const QJsonObject &parameters);
    QJsonObject handleUserUpdate(const QJsonObject &parameters);

    // Product 관련 처리 (ProductManageService 사용)
    QJsonObject handleProductList(const QJsonObject &parameters);
    QJsonObject handleProductGet(const QJsonObject &parameters);
    QJsonObject handleProductCreate(const QJsonObject &parameters);
    QJsonObject handleProductUpdate(const QJsonObject &parameters);
    QJsonObject handleProductDelete(const QJsonObject &parameters);

    // Order 관련 처리 (OrderManageService 사용)
    QJsonObject handleOrderCreate(const QJsonObject &parameters);
    QJsonObject handleOrderGet(const QJsonObject &parameters);
    QJsonObject handleOrderList(const QJsonObject &parameters);
    QJsonObject handleOrderUpdate(const QJsonObject &parameters);

    // OrderItem 관련 처리
    QJsonObject handleOrderItemAdd(const QJsonObject &parameters);
    QJsonObject handleOrderItemUpdate(const QJsonObject &parameters);
    QJsonObject handleOrderItemRemove(const QJsonObject &parameters);

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

    // 데이터 변환 유틸리티
    QJsonObject userToJson(const User* user);
    QJsonObject productToJson(const Product* product);
    QJsonObject orderToJson(const Order* order);
    QJsonObject orderItemToJson(const OrderItem* orderItem);
    QJsonObject chatRoomToJson(const ChatRoom* chatRoom);
    QJsonObject chatUnitToJson(const ChatUnit* chatUnit);
    QJsonArray vectorToJsonArray(const QVector<Product>& products);
    QJsonArray vectorToJsonArray(const QVector<ChatRoom>& products);
    QJsonArray vectorToJsonArray(const QVector<Order>& orders);
};

#endif // PROTOCOLCONTROLLER_H
