// ProtocolController.cpp (userId 기반으로 수정)
#include "ProtocolController.h"
#include "SocketServer.h"
#include <QDebug>
#include <QDateTime>
#include <QUuid>

ProtocolController::ProtocolController(QObject *parent)
    : QObject(parent)
    , socketServer_(nullptr)
{
    // 서비스 인스턴스 가져오기
    userService_ = UserManageService::getInstance();
    chatService_ = ChatManageService::getInstance();
    loginService_ = LoginService::getInstance();
    productService_ = ProductManageService::getInstance();
    orderService_ = OrderManageService::getInstance();

    // 채팅 관련 시그널 연결
    connect(chatService_, &ChatManageService::messageSent,
            this, [this](id_t chatRoomId, id_t chatId) {
                // 채팅 메시지 브로드캐스트
                const ChatUnit* chat = chatService_->getMessageById(chatId);
                if (chat) {
                    broadcastChatMessage(chatRoomId, chat);
                }
            });
}

ProtocolController::~ProtocolController()
{
    userService_->saveUsers();
    productService_->saveProducts();
    orderService_->saveOrders();
    chatService_->saveChatData();
}

bool ProtocolController::initialize(const QString& userFilePath, const QString& productFilePath,
                                    const QString& orderFilePath, const QString& orderItemFilePath,
                                    const QString& chatRoomFilePath, const QString& chatFilePath)
{
    bool success = true;

    success &= userService_->loadUsers(userFilePath);
    success &= chatService_->loadChatData(chatRoomFilePath, chatFilePath);
    success &= productService_->loadProducts(productFilePath);
    success &= orderService_->loadOrders(orderFilePath, orderItemFilePath);

    if (success) {
        qDebug() << "ProtocolController initialized successfully";
    } else {
        qDebug() << "ProtocolController initialization failed";
    }

    return success;
}

QJsonObject ProtocolController::processMessage(const QJsonObject &message)
{
    QJsonObject header = message.value("header").toObject();
    QJsonObject body = message.value("body").toObject();

    QString messageType = header.value("messageType").toString();
    QString messageId = header.value("messageId").toString();

    if (messageType == "command") {
        return processCommand(header, body, ""); // clientId는 별도로 관리 필요
    }

    return createErrorResponse(messageId, "UNSUPPORTED_MESSAGE_TYPE", "Message type not supported");
}

void ProtocolController::onClientMessageReceived(const QString &clientId, const QJsonObject &data)
{
    qDebug() << "Processing message from client:" << clientId;

    if (!validateMessage(data)) {
        QJsonObject errorResponse = createErrorResponse("", "INVALID_MESSAGE", "Invalid message format");
        emit sendToClient(clientId, errorResponse);
        return;
    }

    QJsonObject header = data.value("header").toObject();
    QJsonObject body = data.value("body").toObject();

    QJsonObject response = processCommand(header, body, clientId);
    emit sendToClient(clientId, response);
}

QJsonObject ProtocolController::processCommand(const QJsonObject &header, const QJsonObject &body, const QString &clientId)
{
    QString messageId = header.value("messageId").toString();
    QString action = body.value("action").toString();
    QString target = body.value("target").toString();
    QJsonObject parameters = body.value("parameters").toObject();

    qDebug() << "Processing command:" << action << "on target:" << target;

    // User 관련 처리
    if (target == "user") {
        if (action == "login") return handleUserLogin(parameters, clientId);
        if (action == "logout") return handleUserLogout(parameters, clientId);
        if (action == "register") return handleUserRegister(parameters);
        if (action == "get") return handleUserGet(parameters);
        if (action == "update") return handleUserUpdate(parameters);
    }
    // Product 관련 처리
    else if (target == "product") {
        if (action == "list") return handleProductList(parameters);
        if (action == "get") return handleProductGet(parameters);
        if (action == "create") return handleProductCreate(parameters);
        if (action == "update") return handleProductUpdate(parameters);
        if (action == "delete") return handleProductDelete(parameters);
    }
    // Order 관련 처리
    else if (target == "order") {
        if (action == "create") return handleOrderCreate(parameters);
        if (action == "get") return handleOrderGet(parameters);
        if (action == "list") return handleOrderList(parameters);
        if (action == "update") return handleOrderUpdate(parameters);
    }
    // OrderItem 관련 처리
    else if (target == "orderitem") {
        if (action == "add") return handleOrderItemAdd(parameters);
        if (action == "update") return handleOrderItemUpdate(parameters);
        if (action == "remove") return handleOrderItemRemove(parameters);
    }
    // Chat 관련 처리
    else if (target == "chatroom") {
        if (action == "create") return handleChatRoomCreate(parameters);
        if (action == "join") return handleChatRoomJoin(parameters);
        if (action == "leave") return handleChatRoomLeave(parameters);
        if (action == "list") return handleChatRoomList(parameters);
    }
    else if (target == "chat") {
        if (action == "send") return handleChatSend(parameters, clientId);
        if (action == "history") return handleChatHistory(parameters);
    }

    return createErrorResponse(messageId, "UNKNOWN_COMMAND", "Unknown action or target");
}

QJsonObject ProtocolController::createResponse(const QString &correlationId, const QString &status,
                                               int statusCode, const QString &message,
                                               const QJsonObject &data)
{
    QJsonObject response;
    QJsonObject header;
    QJsonObject body;

    header["messageId"] = generateId();
    header["messageType"] = "response";
    header["timestamp"] = QDateTime::currentSecsSinceEpoch();
    if (!correlationId.isEmpty()) {
        header["correlationId"] = correlationId;
    }

    body["status"] = status;
    body["statusCode"] = statusCode;
    body["message"] = message;
    if (!data.isEmpty()) {
        body["data"] = data;
    }

    response["header"] = header;
    response["body"] = body;

    return response;
}

QJsonObject ProtocolController::createErrorResponse(const QString &correlationId,
                                                    const QString &errorCode,
                                                    const QString &errorMessage)
{
    QJsonObject response;
    QJsonObject header;
    QJsonObject body;
    QJsonObject error;

    header["messageId"] = generateId();
    header["messageType"] = "response";
    header["timestamp"] = QDateTime::currentSecsSinceEpoch();
    if (!correlationId.isEmpty()) {
        header["correlationId"] = correlationId;
    }

    error["code"] = errorCode;
    error["description"] = errorMessage;

    body["status"] = "error";
    body["statusCode"] = 400;
    body["message"] = "Request failed";
    body["error"] = error;

    response["header"] = header;
    response["body"] = body;

    return response;
}

// User 관련 메서드들
QJsonObject ProtocolController::handleUserLogin(const QJsonObject &parameters, const QString &clientId)
{
    QString strId = parameters.value("strId").toString();
    QString password = parameters.value("password").toString();

    QString sessionId;
    User user;
    RaErrorCode result = loginService_->login(strId, password, sessionId, user);

    if (result == Ra_Success) {
        // 클라이언트와 세션 연결
        clientSessions_[clientId] = sessionId;

        // SocketServer에 userId 등록
        QString userId = QString::number(user.getId());
        if (socketServer_) {
            socketServer_->setUserLoggedIn(clientId, userId);
        }

        QJsonObject userData = userToJson(&user);
        userData["sessionId"] = sessionId;
        qDebug() << "User logged in - ID:" << userId << "Session:" << sessionId;

        return createResponse("", "success", 200, "Login successful", userData);
    }

    return createErrorResponse("", "LOGIN_FAILED", "Invalid credentials");
}

QJsonObject ProtocolController::handleUserLogout(const QJsonObject &parameters, const QString &clientId)
{
    Q_UNUSED(parameters)

    if (clientSessions_.contains(clientId)) {
        QString sessionId = clientSessions_[clientId];
        RaErrorCode result = loginService_->logout(sessionId);

        if (result == Ra_Success) {
            clientSessions_.remove(clientId);

            // SocketServer에서 userId 제거
            if (socketServer_) {
                socketServer_->setUserLoggedOut(clientId);
            }

            return createResponse("", "success", 200, "Logout successful");
        }
    }

    return createErrorResponse("", "LOGOUT_FAILED", "Failed to logout");
}

QJsonObject ProtocolController::handleUserRegister(const QJsonObject &parameters)
{
    User newUser;
    if (newUser.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = userService_->createUser(newUser);
        if (result == Ra_Success) {
            QJsonObject userData = userToJson(&newUser);
            return createResponse("", "success", 201, "User registered successfully", userData);
        }
    }

    return createErrorResponse("", "REGISTRATION_FAILED", "Failed to register user");
}

QJsonObject ProtocolController::handleUserGet(const QJsonObject &parameters)
{
    id_t userId = parameters.value("userId").toInteger();

    const User* user = userService_->getUserById(userId);
    if (user) {
        QJsonObject userData = userToJson(user);
        return createResponse("", "success", 200, "User found", userData);
    }

    return createErrorResponse("", "USER_NOT_FOUND", "User not found");
}

QJsonObject ProtocolController::handleUserUpdate(const QJsonObject &parameters)
{
    User user;
    if (user.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = userService_->updateUser(user);
        if (result == Ra_Success) {
            QJsonObject userData = userToJson(&user);
            return createResponse("", "success", 200, "User updated successfully", userData);
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update user");
}

// Product 관련 메서드들
QJsonObject ProtocolController::handleProductList(const QJsonObject &parameters)
{
    Q_UNUSED(parameters)

    QVector<Product> products = productService_->getAllProducts();
    QJsonArray productArray = vectorToJsonArray(products);

    QJsonObject responseData;
    responseData["products"] = productArray;
    responseData["count"] = productArray.size();

    return createResponse("", "success", 200, "Products retrieved successfully", responseData);
}

QJsonObject ProtocolController::handleProductGet(const QJsonObject &parameters)
{
    id_t productId = parameters.value("productId").toInteger();

    const Product* product = productService_->getProductById(productId);
    if (product) {
        return createResponse("", "success", 200, "Product found", productToJson(product));
    }

    return createErrorResponse("", "PRODUCT_NOT_FOUND", "Product not found");
}

QJsonObject ProtocolController::handleProductCreate(const QJsonObject &parameters)
{
    Product newProduct;
    if (newProduct.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = productService_->createProduct(newProduct);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Product created successfully", productToJson(&newProduct));
        }
    }

    return createErrorResponse("", "PRODUCT_CREATION_FAILED", "Failed to create product");
}

QJsonObject ProtocolController::handleProductUpdate(const QJsonObject &parameters)
{
    Product product;
    if (product.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = productService_->updateProduct(product);
        if (result == Ra_Success) {
            return createResponse("", "success", 200, "Product updated successfully", productToJson(&product));
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update product");
}

QJsonObject ProtocolController::handleProductDelete(const QJsonObject &parameters)
{
    id_t productId = parameters.value("productId").toInteger();

    RaErrorCode result = productService_->deleteProduct(productId);
    if (result == Ra_Success) {
        return createResponse("", "success", 200, "Product deleted successfully");
    }

    return createErrorResponse("", "DELETE_FAILED", "Failed to delete product");
}

// Order 관련 메서드들
QJsonObject ProtocolController::handleOrderCreate(const QJsonObject &parameters)
{
    Order newOrder;
    if (newOrder.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = orderService_->createOrder(newOrder);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Order created successfully", orderToJson(&newOrder));
        }
    }

    return createErrorResponse("", "ORDER_CREATION_FAILED", "Failed to create order");
}

QJsonObject ProtocolController::handleOrderGet(const QJsonObject &parameters)
{
    id_t orderId = parameters.value("orderId").toInteger();

    const Order* order = orderService_->getOrderById(orderId);
    if (order) {
        return createResponse("", "success", 200, "Order found", orderToJson(order));
    }

    return createErrorResponse("", "ORDER_NOT_FOUND", "Order not found");
}

QJsonObject ProtocolController::handleOrderList(const QJsonObject &parameters)
{
    id_t userId = parameters.value("userId").toInteger();

    QVector<Order> orders;
    if (userId > 0) {
        orders = orderService_->getOrdersByUserId(userId);
    } else {
        orders = orderService_->getAllOrders();
    }

    QJsonArray orderArray = vectorToJsonArray(orders);

    QJsonObject responseData;
    responseData["orders"] = orderArray;
    responseData["count"] = orderArray.size();

    return createResponse("", "success", 200, "Orders retrieved successfully", responseData);
}

QJsonObject ProtocolController::handleOrderUpdate(const QJsonObject &parameters)
{
    Order order;
    if (order.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = orderService_->updateOrder(order);
        if (result == Ra_Success) {
            return createResponse("", "success", 200, "Order updated successfully", orderToJson(&order));
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update order");
}

// OrderItem 관련 메서드들
QJsonObject ProtocolController::handleOrderItemAdd(const QJsonObject &parameters)
{
    OrderItem newItem;
    if (newItem.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = orderService_->addOrderItem(newItem);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Order item added successfully", orderItemToJson(&newItem));
        }
    }

    return createErrorResponse("", "ORDERITEM_CREATION_FAILED", "Failed to add order item");
}

QJsonObject ProtocolController::handleOrderItemUpdate(const QJsonObject &parameters)
{
    OrderItem item;
    if (item.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = orderService_->updateOrderItem(item);
        if (result == Ra_Success) {
            return createResponse("", "success", 200, "Order item updated successfully", orderItemToJson(&item));
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update order item");
}

QJsonObject ProtocolController::handleOrderItemRemove(const QJsonObject &parameters)
{
    id_t itemId = parameters.value("itemId").toInteger();

    RaErrorCode result = orderService_->removeOrderItem(itemId);
    if (result == Ra_Success) {
        return createResponse("", "success", 200, "Order item removed successfully");
    }

    return createErrorResponse("", "DELETE_FAILED", "Failed to remove order item");
}

// Chat 관련 메서드들
QJsonObject ProtocolController::handleChatRoomCreate(const QJsonObject &parameters)
{
    ChatRoom newRoom;
    if (newRoom.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = chatService_->createChatRoom(newRoom);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Chat room created successfully", chatRoomToJson(&newRoom));
        }
    }

    return createErrorResponse("", "CHATROOM_CREATION_FAILED", "Failed to create chat room");
}

QJsonObject ProtocolController::handleChatRoomJoin(const QJsonObject &parameters)
{
    id_t roomId = parameters.value("chatRoomId").toInteger();
    id_t userId = parameters.value("userId").toInteger();
    qDebug() << QString("Debug :: chatRoomJoin :: roomId(%1) userId(%2)").arg(roomId).arg(userId);
    chatService_->debugPrint();
    if(roomId>0){
        RaErrorCode result = chatService_->addUserToChatRoom(roomId, userId);
        if (result == Ra_Success) {
            const ChatRoom* room = chatService_->getChatRoomById(roomId);
            return createResponse("", "success", 200, "Joined chat room successfully", chatRoomToJson(room));
        }
    }

    return createErrorResponse("", "JOIN_FAILED", "Failed to join chat room");
}

QJsonObject ProtocolController::handleChatRoomLeave(const QJsonObject &parameters)
{
    id_t roomId = parameters.value("chatRoomId").toInteger();
    id_t userId = parameters.value("userId").toInteger();

    RaErrorCode result = chatService_->removeUserFromChatRoom(roomId, userId);
    if (result == Ra_Success) {
        return createResponse("", "success", 200, "Left chat room successfully");
    }

    return createErrorResponse("", "LEAVE_FAILED", "Failed to leave chat room");
}

QJsonObject ProtocolController::handleChatRoomList(const QJsonObject &parameters)
{
    Q_UNUSED(parameters)

    QVector<ChatRoom> rooms = chatService_->getAllChatRooms();
    QJsonArray roomArray = vectorToJsonArray(rooms);

    QJsonObject responseData;
    responseData["rooms"] = roomArray;
    responseData["count"] = roomArray.size();

    return createResponse("", "success", 200, "ChatRooms retrieved successfully", responseData);
}

QJsonObject ProtocolController::handleChatSend(const QJsonObject &parameters, const QString &clientId)
{
    if (!checkAuthentication(clientId)) {
        return createErrorResponse("", "AUTHENTICATION_REQUIRED", "User not authenticated");
    }

    id_t userId = getCurrentUserId(clientId);
    id_t chatRoomId = parameters.value("chatRoomId").toInteger();
    QString chatStr = parameters.value("chatStr").toString();

    // 사용자가 채팅방에 속해있는지 확인
    if (!chatService_->isUserInChatRoom(chatRoomId, userId)) {
        return createErrorResponse("", "ACCESS_DENIED", "User is not in the chat room");
    }

    ChatUnit newChat;
    newChat.setId(-1); // auto-generate

    // ChatUnit에 필요한 데이터 설정
    QJsonObject chatData;
    chatData["userId"] = userId;
    chatData["chatStr"] = chatStr;
    chatData["chatTime"] = QDateTime::currentDateTime().toString();

    newChat.fromJson(chatData);

    RaErrorCode result = chatService_->sendMessage(chatRoomId, newChat);
    if (result == Ra_Success) {
        return createResponse("", "success", 201, "Message sent successfully", chatUnitToJson(&newChat));
    }

    return createErrorResponse("", "CHAT_SEND_FAILED", "Failed to send message");
}

QJsonObject ProtocolController::handleChatHistory(const QJsonObject &parameters)
{
    id_t roomId = parameters.value("chatRoomId").toInteger();
    if(roomId>0){
        QVector<ChatUnit> messages = chatService_->getMessagesByChatRoom(roomId);

        QJsonArray messageArray;
        for (const ChatUnit& chat : messages) {
            messageArray.append(chat.toJson());
        }

        QJsonObject responseData;
        responseData["chatRoomId"] = roomId;
        responseData["messages"] = messageArray;
        responseData["count"] = messageArray.size();

        return createResponse("", "success", 200, "Chat history retrieved successfully", responseData);
    }
    return createErrorResponse("", "CHAT_HISTROY_FAILED", "Failed to send chatHistory");
}

// 채팅 브로드캐스트 처리
void ProtocolController::broadcastChatMessage(id_t chatRoomId, const ChatUnit* chatUnit)
{
    if (!chatUnit || !socketServer_) return;

    // 채팅방의 모든 사용자에게 브로드캐스트
    const ChatRoom* room = chatService_->getChatRoomById(chatRoomId);
    if (!room) return;

    QVector<id_t> userIds = room->getUserIds();
    QStringList userIdStrings;

    for (id_t userId : userIds) {
        userIdStrings.append(QString::number(userId));
    }

    // 브로드캐스트 메시지 생성
    QJsonObject broadcastMsg;
    QJsonObject header;
    header["messageId"] = generateId();
    header["messageType"] = "event";
    header["timestamp"] = QDateTime::currentSecsSinceEpoch();

    QJsonObject body;
    body["eventType"] = "chat_message";
    body["data"] = chatUnitToJson(chatUnit);

    broadcastMsg["header"] = header;
    broadcastMsg["body"] = body;

    // 채팅방 참여자들에게만 브로드캐스트
    emit broadcastToUsers(userIdStrings, broadcastMsg);
}

// 유틸리티 메서드들
bool ProtocolController::validateMessage(const QJsonObject &message)
{
    return message.contains("header") && message.contains("body") &&
           message.value("header").toObject().contains("messageType") &&
           message.value("body").toObject().contains("action") &&
           message.value("body").toObject().contains("target");
}

bool ProtocolController::checkAuthentication(const QString &clientId)
{
    if (!clientSessions_.contains(clientId)) {
        return false;
    }

    QString sessionId = clientSessions_[clientId];
    return loginService_->isSessionValid(sessionId);
}

bool ProtocolController::checkPermission(const QString &clientId, const QString &action, const QString &target)
{
    Q_UNUSED(action)
    Q_UNUSED(target)

    // 간단한 권한 체크 - 로그인된 사용자만 허용
    return checkAuthentication(clientId);
}

id_t ProtocolController::getCurrentUserId(const QString &clientId)
{
    if (!clientSessions_.contains(clientId)) {
        return -1;
    }

    QString sessionId = clientSessions_[clientId];
    return loginService_->getUserBySession(sessionId);
}

// 데이터 변환 유틸리티들
QJsonObject ProtocolController::userToJson(const User* user)
{
    if (!user) return QJsonObject();

    QJsonObject json = user->toJson();
    json.remove("password"); // 보안상 비밀번호 제거
    return json;
}

QJsonObject ProtocolController::productToJson(const Product* product)
{
    return product ? product->toJson() : QJsonObject();
}

QJsonObject ProtocolController::orderToJson(const Order* order)
{
    return order ? order->toJson() : QJsonObject();
}

QJsonObject ProtocolController::orderItemToJson(const OrderItem* orderItem)
{
    return orderItem ? orderItem->toJson() : QJsonObject();
}

QJsonObject ProtocolController::chatRoomToJson(const ChatRoom* chatRoom)
{
    return chatRoom ? chatRoom->toJson() : QJsonObject();
}

QJsonObject ProtocolController::chatUnitToJson(const ChatUnit* chatUnit)
{
    QJsonObject jsonObj = chatUnit ? chatUnit->toJson() : QJsonObject();
    User *user = userService_->getUserById(chatUnit->getUserId());
    QString userName = user->getName();
    jsonObj.insert("userName",userName);
    return jsonObj;
}

QJsonArray ProtocolController::vectorToJsonArray(const QVector<Product>& products)
{
    QJsonArray array;
    for (const Product& product : products) {
        array.append(product.toJson());
    }
    return array;
}

QJsonArray ProtocolController::vectorToJsonArray(const QVector<ChatRoom>& rooms)
{
    QJsonArray array;
    for (const ChatRoom& room : rooms) {
        array.append(room.toJson());
    }
    return array;
}

QJsonArray ProtocolController::vectorToJsonArray(const QVector<Order>& orders)
{
    QJsonArray array;
    for (const Order& order : orders) {
        array.append(order.toJson());
    }
    return array;
}

QString ProtocolController::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
