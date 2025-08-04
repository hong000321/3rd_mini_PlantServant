#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>
#include <QByteArray>

class ClientSocket : public QObject
{
    Q_OBJECT

public:
    explicit ClientSocket(QObject *parent = nullptr);
    ~ClientSocket();

    // 연결 관리
    bool connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;

    // 현재 사용자 정보
    QString currentUserId() const { return m_currentUserId; }
    bool isLoggedIn() const { return !m_currentUserId.isEmpty(); }

    // 인증 관련
    bool login(const QString &strId, const QString &password);
    bool logout();
    bool registerUser(const QString &strId, const QString &password,
                      const QString &name, const QString &email, const QString &address);

    // Product 관련
    bool requestProductList();
    bool requestProduct(int productId);
    bool createProduct(const QString &name, const QString &category, double price, int stock);
    bool updateProduct(int productId, const QString &name, const QString &category, double price, int stock);
    bool deleteProduct(int productId);

    // Order 관련
    bool createOrder(int userId, const QList<QPair<int, int>>& productQuantityList);
    bool requestOrder(int orderId);
    bool requestOrderList(int userId = -1);
    bool updateOrderStatus(int orderId, const QString &status);

    // Chat 관련
    bool createChatRoom(const QString &roomName, const QList<int>& userIds);
    bool joinChatRoom(int chatRoomId, int userId);
    bool leaveChatRoom(int chatRoomId, int userId);
    bool sendChatMessage(int chatRoomId, const QString &message);
    bool requestChatHistory(int chatRoomId);
    bool requestChatRoomList();

    // OrderItem 관련
    bool addOrderItem(int orderId, int productId, int quantity, double unitPrice);
    bool updateOrderItem(int itemId, int quantity, double unitPrice);
    bool removeOrderItem(int itemId);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);

    // 응답 시그널들
    void loginResponse(bool success, const QJsonObject &userData);
    void logoutResponse(bool success);
    void registrationResponse(bool success, const QJsonObject &userData);

    void productListReceived(const QJsonArray &products);
    void productReceived(const QJsonObject &product);
    void productCreated(const QJsonObject &product);
    void productUpdated(const QJsonObject &product);
    void productDeleted(bool success);

    void orderCreated(const QJsonObject &order);
    void orderReceived(const QJsonObject &order);
    void orderListReceived(const QJsonArray &orders);
    void orderUpdated(const QJsonObject &order);

    void orderItemAdded(const QJsonObject &orderItem);
    void orderItemUpdated(const QJsonObject &orderItem);
    void orderItemRemoved(bool success);

    void chatRoomCreated(const QJsonObject &chatRoom);
    void chatRoomJoined(const QJsonObject &chatRoom);
    void chatRoomLeft(bool success);
    void chatRoomListReceived(const QJsonArray &rooms);
    void chatMessageSent(const QJsonObject &message);
    void chatHistoryReceived(const QJsonArray &messages);
    void chatMessageReceived(const QJsonObject &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    QTcpSocket *socket_;
    QByteArray m_buffer;
    quint32 m_expectedPacketSize;
    QString m_sessionId;
    QString m_currentUserId;

    // 최근 요청 추적
    QString m_lastAction;
    QString m_lastTarget;

    // 메시지 처리
    void processIncomingData();
    QByteArray createJsonPacket(const QByteArray &jsonData);
    QString generateMessageId();

    // JSON 메시지 생성
    QJsonObject createCommandMessage(const QString &action, const QString &target,
                                     const QJsonObject &parameters = QJsonObject());
    bool sendMessage(const QJsonObject &message);

    // 응답 처리
    void handleResponse(const QJsonObject &response);
    void handleEvent(const QJsonObject &event);
};

#endif // CLIENTSOCKET_H
