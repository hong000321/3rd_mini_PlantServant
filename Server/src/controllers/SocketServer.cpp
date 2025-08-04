#include "SocketServer.h"
#include <QDebug>
#include <QDataStream>
#include <QDateTime>
#include <QUuid>

// ClientConnection 구현
ClientConnection::ClientConnection(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_expectedPacketSize(0)
{
    generateClientId();
    m_socket->setParent(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientConnection::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &ClientConnection::onSocketError);

    qDebug() << "JSON client connected:" << m_clientId << "from" << clientAddress().toString();
}

ClientConnection::~ClientConnection()
{
    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void ClientConnection::generateClientId()
{
    m_clientId = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

QHostAddress ClientConnection::clientAddress() const
{
    return m_socket ? m_socket->peerAddress() : QHostAddress();
}

quint16 ClientConnection::clientPort() const
{
    return m_socket ? m_socket->peerPort() : 0;
}

bool ClientConnection::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

bool ClientConnection::sendJsonData(const QJsonObject &jsonObject)
{
    QJsonDocument doc(jsonObject);
    return sendJsonData(doc);
}

bool ClientConnection::sendJsonData(const QJsonDocument &jsonDocument)
{
    if (!isConnected()) {
        emit errorOccurred(m_clientId, "Client not connected");
        return false;
    }

    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Compact);
    QByteArray packet = createJsonPacket(jsonData);

    qint64 bytesWritten = m_socket->write(packet);
    bool success = (bytesWritten == packet.size());

    if (success) {
        QString logId = isLoggedIn() ? QString("user:%1").arg(m_userId) : QString("client:%1").arg(m_clientId);
        qDebug() << "JSON sent to" << logId << ":" << jsonData;
    } else {
        emit errorOccurred(m_clientId, "Failed to send JSON data");
    }

    return success;
}

bool ClientConnection::sendJsonString(const QString &jsonString)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(m_clientId, QString("Invalid JSON: %1").arg(parseError.errorString()));
        return false;
    }

    return sendJsonData(doc);
}

QByteArray ClientConnection::createJsonPacket(const QByteArray &jsonData)
{
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<quint32>(jsonData.size());
    packet.append(jsonData);

    return packet;
}

void ClientConnection::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    processIncomingData();
}

void ClientConnection::processIncomingData()
{
    while (true) {
        if (m_expectedPacketSize == 0) {
            // 헤더 읽기 (4바이트 크기 정보)
            if (m_buffer.size() < sizeof(quint32)) {
                break;
            }

            QDataStream stream(m_buffer);
            stream.setByteOrder(QDataStream::BigEndian);
            stream >> m_expectedPacketSize;

            m_buffer.remove(0, sizeof(quint32));
        }

        // JSON 데이터 읽기
        if (m_buffer.size() < static_cast<int>(m_expectedPacketSize)) {
            break;
        }

        QByteArray jsonData = m_buffer.left(m_expectedPacketSize);
        m_buffer.remove(0, m_expectedPacketSize);
        m_expectedPacketSize = 0;

        // JSON 파싱 및 시그널 발생
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QString logId = isLoggedIn() ? QString("user:%1").arg(m_userId) : QString("client:%1").arg(m_clientId);
            qDebug() << "JSON received from" << logId << ":" << jsonData;
            emit jsonDataReceived(m_clientId, doc.object());
        } else {
            emit errorOccurred(m_clientId, QString("JSON parse error: %1").arg(parseError.errorString()));
        }
    }
}

void ClientConnection::onDisconnected()
{
    QString logId = isLoggedIn() ? QString("user:%1 (client:%2)").arg(m_userId, m_clientId) : QString("client:%1").arg(m_clientId);
    qDebug() << "JSON client disconnected:" << logId;

    if (isLoggedIn()) {
        emit userLoggedOut(m_userId);
    }

    emit clientDisconnected(m_clientId);
}

void ClientConnection::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorString = m_socket->errorString();
    QString logId = isLoggedIn() ? QString("user:%1").arg(m_userId) : QString("client:%1").arg(m_clientId);
    qDebug() << "JSON client socket error for" << logId << ":" << errorString;
    emit errorOccurred(m_clientId, errorString);
}

// SocketServer 구현
SocketServer::SocketServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    setupConnections();
}

SocketServer::~SocketServer()
{
    stopServer();
}

void SocketServer::setupConnections()
{
    connect(m_server, &QTcpServer::newConnection, this, &SocketServer::onNewConnection);
}

bool SocketServer::startServer(const QHostAddress &address, quint16 port)
{
    if (m_server->isListening()) {
        emit errorOccurred("JSON server is already listening");
        return false;
    }

    bool success = m_server->listen(address, port);

    if (success) {
        qDebug() << "JSON server started on" << m_server->serverAddress().toString()
        << ":" << m_server->serverPort();
        emit serverStarted(m_server->serverAddress(), m_server->serverPort());
    } else {
        QString error = QString("Failed to start JSON server: %1").arg(m_server->errorString());
        qDebug() << error;
        emit errorOccurred(error);
    }

    return success;
}

bool SocketServer::startServer(quint16 port)
{
    return startServer(QHostAddress::Any, port);
}

void SocketServer::stopServer()
{
    if (!m_server->isListening()) {
        return;
    }

    // 모든 클라이언트 연결 종료
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        it.value()->deleteLater();
    }
    m_clients.clear();
    m_userToClient.clear();
    m_clientToUser.clear();

    m_server->close();
    qDebug() << "JSON server stopped";
    emit serverStopped();
}

bool SocketServer::isListening() const
{
    return m_server->isListening();
}

QStringList SocketServer::getConnectedClients() const
{
    return m_clients.keys();
}

QStringList SocketServer::getLoggedInUsers() const
{
    return m_userToClient.keys();
}

int SocketServer::getClientCount() const
{
    return m_clients.size();
}

int SocketServer::getLoggedInUserCount() const
{
    return m_userToClient.size();
}

ClientConnection* SocketServer::getClient(const QString &clientId) const
{
    return m_clients.value(clientId, nullptr);
}

ClientConnection* SocketServer::getClientByUserId(const QString &userId) const
{
    QString clientId = m_userToClient.value(userId);
    return clientId.isEmpty() ? nullptr : m_clients.value(clientId, nullptr);
}

void SocketServer::setUserLoggedIn(const QString &clientId, const QString &userId)
{
    ClientConnection *client = getClient(clientId);
    if (!client) return;

    // 기존 로그인 정보 정리
    if (m_clientToUser.contains(clientId)) {
        QString oldUserId = m_clientToUser[clientId];
        m_userToClient.remove(oldUserId);
    }

    // 동일한 userId로 다른 클라이언트가 로그인되어 있다면 로그아웃 처리
    if (m_userToClient.contains(userId)) {
        QString oldClientId = m_userToClient[userId];
        ClientConnection *oldClient = getClient(oldClientId);
        if (oldClient) {
            oldClient->setUserId("");
        }
        m_clientToUser.remove(oldClientId);
    }

    // 새로운 로그인 정보 설정
    client->setUserId(userId);
    m_userToClient[userId] = clientId;
    m_clientToUser[clientId] = userId;

    qDebug() << "User logged in:" << userId << "on client:" << clientId;
    emit userLoggedIn(userId, clientId);
}

void SocketServer::setUserLoggedOut(const QString &clientId)
{
    if (!m_clientToUser.contains(clientId)) return;

    QString userId = m_clientToUser[clientId];
    ClientConnection *client = getClient(clientId);

    if (client) {
        client->setUserId("");
    }

    m_userToClient.remove(userId);
    m_clientToUser.remove(clientId);

    qDebug() << "User logged out:" << userId << "from client:" << clientId;
    emit userLoggedOut(userId);
}

// clientId 기반 전송
bool SocketServer::sendJsonToClient(const QString &clientId, const QJsonObject &jsonObject)
{
    ClientConnection *client = getClient(clientId);
    if (!client) {
        emit errorOccurred(QString("Client not found: %1").arg(clientId));
        return false;
    }

    return client->sendJsonData(jsonObject);
}

bool SocketServer::sendJsonToClient(const QString &clientId, const QJsonDocument &jsonDocument)
{
    ClientConnection *client = getClient(clientId);
    if (!client) {
        emit errorOccurred(QString("Client not found: %1").arg(clientId));
        return false;
    }

    return client->sendJsonData(jsonDocument);
}

bool SocketServer::sendJsonToClient(const QString &clientId, const QString &jsonString)
{
    ClientConnection *client = getClient(clientId);
    if (!client) {
        emit errorOccurred(QString("Client not found: %1").arg(clientId));
        return false;
    }

    return client->sendJsonString(jsonString);
}

// userId 기반 전송
bool SocketServer::sendJsonToUser(const QString &userId, const QJsonObject &jsonObject)
{
    ClientConnection *client = getClientByUserId(userId);
    if (!client) {
        return false;
    }

    return client->sendJsonData(jsonObject);
}

bool SocketServer::sendJsonToUser(const QString &userId, const QJsonDocument &jsonDocument)
{
    ClientConnection *client = getClientByUserId(userId);
    if (!client) {
        emit errorOccurred(QString("User not found or not logged in: %1").arg(userId));
        return false;
    }

    return client->sendJsonData(jsonDocument);
}

bool SocketServer::sendJsonToUser(const QString &userId, const QString &jsonString)
{
    ClientConnection *client = getClientByUserId(userId);
    if (!client) {
        emit errorOccurred(QString("User not found or not logged in: %1").arg(userId));
        return false;
    }

    return client->sendJsonString(jsonString);
}

void SocketServer::broadcastJson(const QJsonObject &jsonObject)
{
    for (auto client : m_clients.values()) {
        client->sendJsonData(jsonObject);
    }
}

void SocketServer::broadcastJson(const QJsonDocument &jsonDocument)
{
    for (auto client : m_clients.values()) {
        client->sendJsonData(jsonDocument);
    }
}

void SocketServer::broadcastJson(const QString &jsonString)
{
    for (auto client : m_clients.values()) {
        client->sendJsonString(jsonString);
    }
}

void SocketServer::broadcastToUsers(const QStringList &userIds, const QJsonObject &jsonObject)
{
    for (const QString &userId : userIds) {
        sendJsonToUser(userId, jsonObject);
    }
}

QHostAddress SocketServer::serverAddress() const
{
    return m_server->serverAddress();
}

quint16 SocketServer::serverPort() const
{
    return m_server->serverPort();
}

QString SocketServer::getServerInfo() const
{
    if (isListening()) {
        return QString("JSON server listening on %1:%2 (%3 clients, %4 logged in)")
        .arg(serverAddress().toString())
            .arg(serverPort())
            .arg(getClientCount())
            .arg(getLoggedInUserCount());
    } else {
        return "JSON server is not running";
    }
}

void SocketServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        ClientConnection *client = new ClientConnection(socket, this);

        connect(client, &ClientConnection::clientDisconnected,
                this, &SocketServer::onClientDisconnected);
        connect(client, &ClientConnection::userLoggedOut,
                this, &SocketServer::onUserLoggedOut);
        connect(client, &ClientConnection::jsonDataReceived,
                this, &SocketServer::onClientJsonReceived);
        connect(client, &ClientConnection::errorOccurred,
                this, &SocketServer::onClientError);

        m_clients[client->clientId()] = client;

        emit clientConnected(client->clientId(), client->clientAddress());
    }
}

void SocketServer::onClientDisconnected(const QString &clientId)
{
    if (m_clients.contains(clientId)) {
        // 로그인 정보 정리
        if (m_clientToUser.contains(clientId)) {
            QString userId = m_clientToUser[clientId];
            m_userToClient.remove(userId);
            m_clientToUser.remove(clientId);
        }

        m_clients[clientId]->deleteLater();
        m_clients.remove(clientId);
        emit clientDisconnected(clientId);
    }
}

void SocketServer::onUserLoggedOut(const QString &userId)
{
    emit userLoggedOut(userId);
}

void SocketServer::onClientJsonReceived(const QString &clientId, const QJsonObject &data)
{
    emit jsonDataReceived(clientId, data);
}

void SocketServer::onClientError(const QString &clientId, const QString &error)
{
    emit errorOccurred(QString("Client %1: %2").arg(clientId, error));
}
