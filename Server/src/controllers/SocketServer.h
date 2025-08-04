#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>
#include <QMap>

class ClientConnection : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnection(QTcpSocket *socket, QObject *parent = nullptr);
    ~ClientConnection();

    QString clientId() const { return m_clientId; }
    QString userId() const { return m_userId; }
    void setUserId(const QString &userId) { m_userId = userId; }
    bool isLoggedIn() const { return !m_userId.isEmpty(); }

    QHostAddress clientAddress() const;
    quint16 clientPort() const;
    bool isConnected() const;

    bool sendJsonData(const QJsonObject &jsonObject);
    bool sendJsonData(const QJsonDocument &jsonDocument);
    bool sendJsonString(const QString &jsonString);

signals:
    void jsonDataReceived(const QString &clientId, const QJsonObject &data);
    void clientDisconnected(const QString &clientId);
    void userLoggedOut(const QString &userId);
    void errorOccurred(const QString &clientId, const QString &error);

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    QTcpSocket *m_socket;
    QString m_clientId;     // 연결 식별용 (임시 ID)
    QString m_userId;       // 로그인 후 실제 사용자 ID
    QByteArray m_buffer;
    quint32 m_expectedPacketSize;

    void processIncomingData();
    QByteArray createJsonPacket(const QByteArray &jsonData);
    void generateClientId();
};

class SocketServer : public QObject
{
    Q_OBJECT

public:
    explicit SocketServer(QObject *parent = nullptr);
    ~SocketServer();

    // 서버 제어
    bool startServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 5105);
    bool startServer(quint16 port = 5105);
    void stopServer();
    bool isListening() const;

    // 클라이언트 관리
    QStringList getConnectedClients() const;
    QStringList getLoggedInUsers() const;
    int getClientCount() const;
    int getLoggedInUserCount() const;
    ClientConnection* getClient(const QString &clientId) const;
    ClientConnection* getClientByUserId(const QString &userId) const;

    // 사용자 로그인/로그아웃 관리
    void setUserLoggedIn(const QString &clientId, const QString &userId);
    void setUserLoggedOut(const QString &clientId);

    // 데이터 전송 (clientId 기반)
    bool sendJsonToClient(const QString &clientId, const QJsonObject &jsonObject);
    bool sendJsonToClient(const QString &clientId, const QJsonDocument &jsonDocument);
    bool sendJsonToClient(const QString &clientId, const QString &jsonString);

    // 데이터 전송 (userId 기반)
    bool sendJsonToUser(const QString &userId, const QJsonObject &jsonObject);
    bool sendJsonToUser(const QString &userId, const QJsonDocument &jsonDocument);
    bool sendJsonToUser(const QString &userId, const QString &jsonString);

    // 브로드캐스트
    void broadcastJson(const QJsonObject &jsonObject);
    void broadcastJson(const QJsonDocument &jsonDocument);
    void broadcastJson(const QString &jsonString);
    void broadcastToUsers(const QStringList &userIds, const QJsonObject &jsonObject);

    // 서버 정보
    QHostAddress serverAddress() const;
    quint16 serverPort() const;
    QString getServerInfo() const;

signals:
    void clientConnected(const QString &clientId, const QHostAddress &address);
    void clientDisconnected(const QString &clientId);
    void userLoggedIn(const QString &userId, const QString &clientId);
    void userLoggedOut(const QString &userId);
    void jsonDataReceived(const QString &clientId, const QJsonObject &data);
    void serverStarted(const QHostAddress &address, quint16 port);
    void serverStopped();
    void errorOccurred(const QString &error);

private slots:
    void onNewConnection();
    void onClientDisconnected(const QString &clientId);
    void onUserLoggedOut(const QString &userId);
    void onClientJsonReceived(const QString &clientId, const QJsonObject &data);
    void onClientError(const QString &clientId, const QString &error);

private:
    QTcpServer *m_server;
    QMap<QString, ClientConnection*> m_clients;        // clientId -> ClientConnection
    QMap<QString, QString> m_userToClient;             // userId -> clientId
    QMap<QString, QString> m_clientToUser;             // clientId -> userId

    void setupConnections();
};

#endif // SOCKETSERVER_H
