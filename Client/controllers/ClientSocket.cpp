#include "ClientSocket.h"
#include <QDebug>
#include <QDateTime>
#include <QUuid>
#include <QThread>
ClientSocket::ClientSocket(QObject *parent)
    : QObject(parent)
    , socket_(new QTcpSocket(this))
    , m_expectedPacketSize(0)
{
    connect(socket_, &QTcpSocket::connected, this, &ClientSocket::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &ClientSocket::onDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &ClientSocket::onReadyRead);
    connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &ClientSocket::onSocketError);
}

ClientSocket::~ClientSocket()
{
    if (socket_->state() != QAbstractSocket::UnconnectedState) {
        socket_->disconnectFromHost();
    }
}

bool ClientSocket::connectToServer(const QString &host, quint16 port)
{
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Already connected to server";
        return true;
    }

    qDebug() << "Connecting to JSON server:" << host << ":" << port;
    socket_->connectToHost(host, port);

    return socket_->waitForConnected(5000);
}

void ClientSocket::disconnectFromServer()
{
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->disconnectFromHost();
    }
}

bool ClientSocket::isConnected() const
{
    return socket_->state() == QAbstractSocket::ConnectedState;
}

// 인증 관련 메서드들
bool ClientSocket::login(const QString &strId, const QString &password)
{
    QJsonObject parameters;
    parameters["strId"] = strId;
    parameters["password"] = password;

    QJsonObject message = createCommandMessage("login", "user", parameters);
    return sendMessage(message);
}

bool ClientSocket::logout()
{
    QJsonObject message = createCommandMessage("logout", "user");
    return sendMessage(message);
}

bool ClientSocket::registerUser(const QString &strId, const QString &password,
                                const QString &name, const QString &email, const QString &address)
{
    QJsonObject parameters;
    parameters["strId"] = strId;
    parameters["password"] = password;
    parameters["name"] = name;
    parameters["email"] = email;
    parameters["address"] = address;
    parameters["level"] = 2; // 일반 사용자

    QJsonObject message = createCommandMessage("register", "user", parameters);
    return sendMessage(message);
}

// Product 관련 메서드들
bool ClientSocket::requestProductList()
{
    QJsonObject message = createCommandMessage("list", "product");
    return sendMessage(message);
}

bool ClientSocket::requestProduct(int productId)
{
    QJsonObject parameters;
    parameters["productId"] = productId;

    QJsonObject message = createCommandMessage("get", "product", parameters);
    return sendMessage(message);
}

bool ClientSocket::createProduct(const QString &name, const QString &category, double price, int stock)
{
    QJsonObject parameters;
    parameters["name"] = name;
    parameters["category"] = category;
    parameters["price"] = price;
    parameters["stock"] = stock;

    QJsonObject message = createCommandMessage("create", "product", parameters);
    return sendMessage(message);
}

bool ClientSocket::updateProduct(int productId, const QString &name, const QString &category, double price, int stock)
{
    QJsonObject parameters;
    parameters["productId"] = productId;
    parameters["name"] = name;
    parameters["category"] = category;
    parameters["price"] = price;
    parameters["stock"] = stock;

    QJsonObject message = createCommandMessage("update", "product", parameters);
    return sendMessage(message);
}

bool ClientSocket::deleteProduct(int productId)
{
    QJsonObject parameters;
    parameters["productId"] = productId;

    QJsonObject message = createCommandMessage("delete", "product", parameters);
    return sendMessage(message);
}

// Order 관련 메서드들
bool ClientSocket::createOrder(int userId, const QList<QPair<int, int>>& productQuantityList)
{
    QJsonObject parameters;
    parameters["userId"] = userId > 0 ? userId : m_currentUserId.toInt();
    parameters["orderDate"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // OrderItems 생성
    QJsonArray orderItems;
    for (int i = 0; i < productQuantityList.size(); ++i) {
        QJsonObject item;
        item["productId"] = productQuantityList[i].first;
        item["quantity"] = productQuantityList[i].second;
        orderItems.append(item);
    }
    parameters["orderItems"] = orderItems;

    QJsonObject message = createCommandMessage("create", "order", parameters);
    return sendMessage(message);
}

bool ClientSocket::requestOrder(int orderId)
{
    QJsonObject parameters;
    parameters["orderId"] = orderId;

    QJsonObject message = createCommandMessage("get", "order", parameters);
    return sendMessage(message);
}

bool ClientSocket::requestOrderList(int userId)
{
    QJsonObject parameters;
    if (userId > 0) {
        parameters["userId"] = userId;
    } else if (!m_currentUserId.isEmpty()) {
        parameters["userId"] = m_currentUserId.toInt();
    }
    // userId가 -1이거나 0인 경우 parameters가 비어있어서 전체 목록 요청

    QJsonObject message = createCommandMessage("list", "order", parameters);
    return sendMessage(message);
}

bool ClientSocket::updateOrderStatus(int orderId, const QString &status)
{
    QJsonObject parameters;
    parameters["orderId"] = orderId;
    parameters["status"] = status;

    QJsonObject message = createCommandMessage("update", "order", parameters);
    return sendMessage(message);
}

// Chat 관련 메서드들
bool ClientSocket::createChatRoom(const QString &roomName, const QList<int>& userIds)
{
    QJsonObject parameters;
    parameters["chatRoomName"] = roomName;

    QJsonObject userIdsObj;
    for (int i = 0; i < userIds.size(); ++i) {
        userIdsObj[QString("userId%1").arg(i)] = userIds[i];
    }
    parameters["userIds"] = userIdsObj;

    QJsonObject message = createCommandMessage("create", "chatroom", parameters);
    return sendMessage(message);
}

bool ClientSocket::joinChatRoom(int chatRoomId, int userId)
{
    QJsonObject parameters;
    parameters["chatRoomId"] = chatRoomId;
    parameters["userId"] = userId > 0 ? userId : m_currentUserId.toInt();

    QJsonObject message = createCommandMessage("join", "chatroom", parameters);
    return sendMessage(message);
}

bool ClientSocket::leaveChatRoom(int chatRoomId, int userId)
{
    QJsonObject parameters;
    parameters["chatRoomId"] = chatRoomId;
    parameters["userId"] = userId > 0 ? userId : m_currentUserId.toInt();

    QJsonObject message = createCommandMessage("leave", "chatroom", parameters);
    return sendMessage(message);
}

bool ClientSocket::sendChatMessage(int chatRoomId, const QString &message)
{
    QJsonObject parameters;
    parameters["chatRoomId"] = chatRoomId;
    parameters["chatStr"] = message;

    QJsonObject msg = createCommandMessage("send", "chat", parameters);
    return sendMessage(msg);
}

bool ClientSocket::requestChatHistory(int chatRoomId)
{
    QJsonObject parameters;
    parameters["chatRoomId"] = chatRoomId;

    QJsonObject message = createCommandMessage("history", "chat", parameters);
    return sendMessage(message);
}

bool ClientSocket::requestChatRoomList(){
    QJsonObject message = createCommandMessage("list", "chatroom");
    return sendMessage(message);
}


// OrderItem 관련 메서드들
bool ClientSocket::addOrderItem(int orderId, int productId, int quantity, double unitPrice)
{
    QJsonObject parameters;
    parameters["orderId"] = orderId;
    parameters["productId"] = productId;
    parameters["quantity"] = quantity;
    parameters["unitPrice"] = unitPrice;

    QJsonObject message = createCommandMessage("add", "orderitem", parameters);
    return sendMessage(message);
}

bool ClientSocket::updateOrderItem(int itemId, int quantity, double unitPrice)
{
    QJsonObject parameters;
    parameters["itemId"] = itemId;
    parameters["quantity"] = quantity;
    parameters["unitPrice"] = unitPrice;

    QJsonObject message = createCommandMessage("update", "orderitem", parameters);
    return sendMessage(message);
}

bool ClientSocket::removeOrderItem(int itemId)
{
    QJsonObject parameters;
    parameters["itemId"] = itemId;

    QJsonObject message = createCommandMessage("remove", "orderitem", parameters);
    return sendMessage(message);
}

// 네트워크 이벤트 핸들러들
void ClientSocket::onConnected()
{
    qDebug() << "Connected to JSON server";
    emit connected();
}

void ClientSocket::onDisconnected()
{
    qDebug() << "Disconnected from JSON server";
    m_sessionId.clear();
    m_currentUserId.clear();
    m_lastAction.clear();
    m_lastTarget.clear();
    emit disconnected();
}

void ClientSocket::onReadyRead()
{
    m_buffer.append(socket_->readAll());
    processIncomingData();
}

void ClientSocket::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorString = socket_->errorString();
    qDebug() << "JSON socket error:" << errorString;
    emit errorOccurred(errorString);
}

// 메시지 처리 메서드들
void ClientSocket::processIncomingData()
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

        // JSON 파싱 및 처리
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            QJsonObject message = doc.object();
            QJsonObject header = message.value("header").toObject();
            QString messageType = header.value("messageType").toString();

            if (messageType == "response") {
                handleResponse(message);
            } else if (messageType == "event") {
                handleEvent(message);
            }
        } else {
            qDebug() << "JSON parse error:" << parseError.errorString();
        }
    }
}

//json packet만들기
QByteArray ClientSocket::createJsonPacket(const QByteArray &jsonData)
{
    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << static_cast<quint32>(jsonData.size());
    packet.append(jsonData);

    return packet;
}


QString ClientSocket::generateMessageId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

/* createCommandMessage */
QJsonObject ClientSocket::createCommandMessage(const QString &action, const QString &target,
                                               const QJsonObject &parameters)
{
    QJsonObject message;
    QJsonObject header;
    QJsonObject body;

    header["messageId"] = generateMessageId();
    header["messageType"] = "command";
    header["timestamp"] = QDateTime::currentSecsSinceEpoch();

    body["action"] = action;
    body["target"] = target;
    if (!parameters.isEmpty()) {
        body["parameters"] = parameters;
    }

    message["header"] = header;
    message["body"] = body;

    // 마지막 요청 정보 저장 (간단한 추적)
    m_lastAction = action;
    m_lastTarget = target;

    return message;
}

//서버로 sendMessage
bool ClientSocket::sendMessage(const QJsonObject &message)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return false;
    }

    QJsonDocument doc(message);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    QByteArray packet = createJsonPacket(jsonData);

    qint64 bytesWritten = socket_->write(packet);
    bool success = (bytesWritten == packet.size());

    if (success) {
        qDebug() << "JSON message sent:" << jsonData;
    } else {
        emit errorOccurred("Failed to send message");
    }
    // QThread::msleep(100);
    return success;
}

void ClientSocket::handleResponse(const QJsonObject &response)
{
    QJsonObject header = response.value("header").toObject();
    QJsonObject body = response.value("body").toObject();

    QString status = body.value("status").toString();
    bool success = (status == "success");
    QJsonObject data = body.value("data").toObject();

    // 데이터 내용과 최근 요청 정보를 조합해서 응답 타입 결정
    QString target = m_lastTarget;
    QString action = m_lastAction;

    // 데이터 내용으로 더 정확한 판단 가능한 경우 우선 적용
    if (data.contains("sessionId")) {
        // 명확히 로그인 응답
        target = "user";
        action = "login";
    } else if (data.contains("products")) {
        // 제품 목록 응답
        target = "product";
        action = "list";
    } else if (data.contains("orders")) {
        // 주문 목록 응답
        target = "order";
        action = "list";
    } else if (data.contains("messages")) {
        // 채팅 기록 응답
        target = "chat";
        action = "history";
    }

    // target과 action을 기반으로 응답 처리
    if (target == "user") {
        if (action == "login") {
            if (success) {
                m_sessionId = data.value("sessionId").toString();
                m_currentUserId = QString::number(data.value("userId").toInt());
            }
            emit loginResponse(success, data);
        } else if (action == "logout") {
            if (success) {
                m_sessionId.clear();
                m_currentUserId.clear();
            }
            emit logoutResponse(success);
        } else if (action == "register") {
            emit registrationResponse(success, data);
        } else if (action == "get") {
            if (success) {
                emit productReceived(data); // User 정보 표시용 (또는 별도 시그널 추가)
            }
        } else if (action == "update") {
            if (success) {
                emit registrationResponse(success, data); // 사용자 정보 업데이트 성공
            }
        }
    }
    else if (target == "product") {
        if (action == "list") {
            if (success && data.contains("products")) {
                emit productListReceived(data.value("products").toArray());
            }
        } else if (action == "get") {
            if (success) {
                emit productReceived(data);
            }
        } else if (action == "create") {
            if (success) {
                emit productCreated(data);
            }
        } else if (action == "update") {
            if (success) {
                emit productUpdated(data);
            }
        } else if (action == "delete") {
            emit productDeleted(success);
        }
    }

    else if (target == "chatroom") {
        if (action == "create") {
            if (success) {
                emit chatRoomCreated(data);
            }
        } else if (action == "join") {
            if (success) {
                emit chatRoomJoined(data);
            }
        } else if (action == "leave") {
            emit chatRoomLeft(success);
        } else if (action == "list" && data.contains("rooms")) {
            emit chatRoomListReceived(data.value("rooms").toArray());
        }
    }
    else if (target == "chat") {
        if (action == "send") {
            if (success) {
                emit chatMessageSent(data);
            }
        } else if (action == "history") {
            if (success && data.contains("messages")) {
                emit chatHistoryReceived(data.value("messages").toArray());
            }
        }
    }
    else {
        // 알 수 없는 target/action이거나 명확하지 않은 경우 데이터 기반 fallback
        if (data.contains("productId") && !data.contains("products")) {
            // 단일 제품 응답
            emit productReceived(data);
        } else if (data.contains("orderId") && !data.contains("orders")) {
            // 단일 주문 응답
            emit orderReceived(data);
        } else if (data.contains("chatRoomName") && !data.contains("messages")) {
            // 채팅방 생성/참여 응답
            emit chatRoomCreated(data);
        }
    }

    // 에러 처리
    if (!success) {
        QJsonObject error = body.value("error").toObject();
        QString errorCode = error.value("code").toString();
        QString errorDescription = error.value("description").toString();
        QString errorMessage = QString("Error %1: %2").arg(errorCode, errorDescription);
        emit errorOccurred(errorMessage);
    }
}

void ClientSocket::handleEvent(const QJsonObject &event)
{
    QJsonObject body = event.value("body").toObject();
    QString eventType = body.value("eventType").toString();
    QJsonObject data = body.value("data").toObject();

    if (eventType == "chat_message") {
        emit chatMessageReceived(data);
    }
    // 기타 이벤트들도 처리...
}
