#include "ProtocolController.h"
#include "SocketServer.h"
#include <QDebug>
#include <QDateTime>
#include <QUuid>

ProtocolController::ProtocolController(QObject *parent)
    : QObject(parent)
    , socketServer_(nullptr)
{
    // 서비스 인스턴스 가져오기 (PostManageService 추가)
    userService_ = UserManageService::getInstance();
    chatService_ = ChatManageService::getInstance();
    loginService_ = LoginService::getInstance();
    plantService_ = PlantManageService::getInstance();
    postService_ = PostManageService::getInstance();

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
    plantService_->savePlants();
    postService_->savePosts();
    chatService_->saveChatData();
}

bool ProtocolController::initialize(const QString& userFilePath, const QString& plantFilePath,
                                    const QString& postFilePath, const QString& chatRoomFilePath,
                                    const QString& chatFilePath)
{
    bool success = true;

    success &= userService_->loadUsers(userFilePath);
    success &= chatService_->loadChatData(chatRoomFilePath, chatFilePath);
    success &= plantService_->loadPlants(plantFilePath);
    success &= postService_->loadPosts(postFilePath);

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
    // Plant 관련 처리
    else if (target == "plant") {
        if (action == "list") return handlePlantList(parameters);
        if (action == "get") return handlePlantGet(parameters);
        if (action == "create") return handlePlantCreate(parameters);
        if (action == "update") return handlePlantUpdate(parameters);
        if (action == "delete") return handlePlantDelete(parameters);
        if (action == "sensor_update") return handlePlantSensorUpdate(parameters);
    }
    // Post 관련 처리
    else if (target == "post") {
        if (action == "list") return handlePostList(parameters);
        if (action == "get") return handlePostGet(parameters);
        if (action == "create") return handlePostCreate(parameters,clientId);
        if (action == "update") return handlePostUpdate(parameters,clientId);
        if (action == "delete") return handlePostDelete(parameters,clientId);
        if (action == "search") return handlePostSearch(parameters);
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

// Plant 관련 메서드들
QJsonObject ProtocolController::handlePlantList(const QJsonObject &parameters)
{
    id_t userId = parameters.value("userId").toInteger();

    QVector<Plant> plants;
    if (userId > 0) {
        plants = plantService_->getPlantsByUserId(userId);
    } else {
        plants = plantService_->getAllPlants();
    }

    QJsonArray plantArray = vectorToJsonArray(plants);

    QJsonObject responseData;
    responseData["plants"] = plantArray;
    responseData["count"] = plantArray.size();

    return createResponse("", "success", 200, "Plants retrieved successfully", responseData);
}

QJsonObject ProtocolController::handlePlantGet(const QJsonObject &parameters)
{
    id_t plantId = parameters.value("plantId").toInteger();

    const Plant* plant = plantService_->getPlantById(plantId);
    if (plant) {
        return createResponse("", "success", 200, "Plant found", plantToJson(plant));
    }

    return createErrorResponse("", "PLANT_NOT_FOUND", "Plant not found");
}

QJsonObject ProtocolController::handlePlantCreate(const QJsonObject &parameters)
{
    Plant newPlant;
    if (newPlant.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = plantService_->createPlant(newPlant);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Plant created successfully", plantToJson(&newPlant));
        }
    }

    return createErrorResponse("", "PLANT_CREATION_FAILED", "Failed to create plant");
}

QJsonObject ProtocolController::handlePlantUpdate(const QJsonObject &parameters)
{
    Plant plant;
    if (plant.fromJson(parameters) == Ra_Success) {
        RaErrorCode result = plantService_->updatePlant(plant);
        if (result == Ra_Success) {
            return createResponse("", "success", 200, "Plant updated successfully", plantToJson(&plant));
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update plant");
}

QJsonObject ProtocolController::handlePlantDelete(const QJsonObject &parameters)
{
    id_t plantId = parameters.value("plantId").toInteger();

    RaErrorCode result = plantService_->deletePlant(plantId);
    if (result == Ra_Success) {
        return createResponse("", "success", 200, "Plant deleted successfully");
    }

    return createErrorResponse("", "DELETE_FAILED", "Failed to delete plant");
}

QJsonObject ProtocolController::handlePlantSensorUpdate(const QJsonObject &parameters)
{
    id_t plantId = parameters.value("plantId").toInteger();
    int humidity = parameters.value("humidity").toInt();
    double temperature = parameters.value("temperature").toDouble();

    RaErrorCode result = plantService_->updateSensorData(plantId, humidity, temperature);
    if (result == Ra_Success) {
        const Plant* updatedPlant = plantService_->getPlantById(plantId);
        return createResponse("", "success", 200, "Sensor data updated successfully", plantToJson(updatedPlant));
    }

    return createErrorResponse("", "SENSOR_UPDATE_FAILED", "Failed to update sensor data");
}

// Post 관련 메서드들
QJsonObject ProtocolController::handlePostList(const QJsonObject &parameters)
{
    id_t authorId = parameters.value("authorId").toInteger();
    int page = parameters.value("page").toInt();
    int pageSize = parameters.value("pageSize").toInt();
    QString sortTypeStr = parameters.value("sortType").toString();

    // 기본값 설정
    if (pageSize <= 0 || pageSize > 100) pageSize = 20; // 최대 100개로 제한
    if (page <= 0) page = 1;

    // 정렬 타입 변환
    PostSortType sortType = PostSortType::CreatedDate_Desc;
    if (sortTypeStr == "created_asc") sortType = PostSortType::CreatedDate_Asc;
    else if (sortTypeStr == "updated_desc") sortType = PostSortType::UpdatedDate_Desc;
    else if (sortTypeStr == "updated_asc") sortType = PostSortType::UpdatedDate_Asc;
    else if (sortTypeStr == "title_asc") sortType = PostSortType::Title_Asc;
    else if (sortTypeStr == "title_desc") sortType = PostSortType::Title_Desc;

    QVector<Post> posts;
    int totalCount = 0;

    if (authorId > 0) {
        // 특정 작성자의 게시글
        posts = postService_->getPostsByAuthorIdSorted(authorId, sortType);
        totalCount = posts.size();

        // 페이징 처리
        int startIndex = (page - 1) * pageSize;
        int endIndex = qMin(startIndex + pageSize, posts.size());
        if (startIndex < posts.size()) {
            posts = posts.mid(startIndex, endIndex - startIndex);
        } else {
            posts.clear();
        }
    } else {
        // 전체 게시글 (페이징 적용)
        totalCount = postService_->getTotalPostCount();
        posts = postService_->getPostsPaginated(page, pageSize, sortType);
    }

    QJsonArray postArray = vectorToJsonArray(posts);

    QJsonObject responseData;
    responseData["posts"] = postArray;
    responseData["count"] = postArray.size();
    responseData["totalCount"] = totalCount;
    responseData["page"] = page;
    responseData["pageSize"] = pageSize;
    responseData["totalPages"] = (totalCount + pageSize - 1) / pageSize;

    return createResponse("", "success", 200, "Posts retrieved successfully", responseData);
}

QJsonObject ProtocolController::handlePostGet(const QJsonObject &parameters)
{
    id_t postId = parameters.value("postId").toInteger();

    const Post* post = postService_->getPostById(postId);
    if (post) {
        return createResponse("", "success", 200, "Post found", postToJson(post));
    }

    return createErrorResponse("", "POST_NOT_FOUND", "Post not found");
}

QJsonObject ProtocolController::handlePostCreate(const QJsonObject &parameters, const QString &clientId)
{
    // 인증 확인
    if (!checkAuthentication(clientId)) {
        return createErrorResponse("", "AUTHENTICATION_REQUIRED", "User not authenticated");
    }

    Post newPost;
    if (newPost.fromJson(parameters) == Ra_Success) {
        // 작성자 ID가 현재 로그인된 사용자와 일치하는지 확인
        id_t currentUserId = getCurrentUserId(clientId);
        if (newPost.getUserId() != currentUserId) {
            newPost.setUserId(currentUserId); // 현재 사용자로 강제 설정
        }

        RaErrorCode result = postService_->createPost(newPost);
        if (result == Ra_Success) {
            return createResponse("", "success", 201, "Post created successfully", postToJson(&newPost));
        }
    }

    return createErrorResponse("", "POST_CREATION_FAILED", "Failed to create post");
}

QJsonObject ProtocolController::handlePostUpdate(const QJsonObject &parameters, const QString &clientId)
{
    if (!checkAuthentication(clientId)) {
        return createErrorResponse("", "AUTHENTICATION_REQUIRED", "User not authenticated");
    }

    Post post;
    if (post.fromJson(parameters) == Ra_Success) {
        // 기존 게시글 확인
        const Post* existingPost = postService_->getPostById(post.getId());
        if (!existingPost) {
            return createErrorResponse("", "POST_NOT_FOUND", "Post not found");
        }

        // 작성자 본인만 수정 가능
        id_t currentUserId = getCurrentUserId(clientId);
        if (existingPost->getUserId() != currentUserId) {
            return createErrorResponse("", "ACCESS_DENIED", "Only author can update the post");
        }

        RaErrorCode result = postService_->updatePost(post);
        if (result == Ra_Success) {
            return createResponse("", "success", 200, "Post updated successfully", postToJson(&post));
        }
    }

    return createErrorResponse("", "UPDATE_FAILED", "Failed to update post");
}

QJsonObject ProtocolController::handlePostDelete(const QJsonObject &parameters, const QString &clientId)
{
    if (!checkAuthentication(clientId)) {
        return createErrorResponse("", "AUTHENTICATION_REQUIRED", "User not authenticated");
    }

    id_t postId = parameters.value("postId").toInteger();

    // 기존 게시글 확인
    const Post* existingPost = postService_->getPostById(postId);
    if (!existingPost) {
        return createErrorResponse("", "POST_NOT_FOUND", "Post not found");
    }

    // 작성자 본인만 삭제 가능
    id_t currentUserId = getCurrentUserId(clientId);
    if (existingPost->getUserId() != currentUserId) {
        return createErrorResponse("", "ACCESS_DENIED", "Only author can delete the post");
    }

    RaErrorCode result = postService_->deletePost(postId);
    if (result == Ra_Success) {
        return createResponse("", "success", 200, "Post deleted successfully");
    }

    return createErrorResponse("", "DELETE_FAILED", "Failed to delete post");
}

QJsonObject ProtocolController::handlePostSearch(const QJsonObject &parameters)
{
    QString keyword = parameters.value("keyword").toString();
    QString searchType = parameters.value("searchType").toString(); // "title", "content", "both"

    if (keyword.isEmpty()) {
        return createErrorResponse("", "INVALID_PARAMETER", "Keyword is required");
    }

    QVector<Post> posts;

    if (searchType == "title") {
        posts = postService_->searchPostsByTitle(keyword);
    } else if (searchType == "content") {
        posts = postService_->searchPostsByContent(keyword);
    } else {
        // 기본값: 제목과 내용 모두 검색
        posts = postService_->searchPostsByTitleOrContent(keyword);
    }

    QJsonArray postArray = vectorToJsonArray(posts);

    QJsonObject responseData;
    responseData["posts"] = postArray;
    responseData["count"] = postArray.size();
    responseData["keyword"] = keyword;
    responseData["searchType"] = searchType.isEmpty() ? "both" : searchType;

    return createResponse("", "success", 200, "Search completed successfully", responseData);
}

// Post 관련 데이터 변환 유틸리티
QJsonObject ProtocolController::postToJson(const Post* post)
{
    if (!post) return QJsonObject();

    QJsonObject jsonObj = post->toJson();

    // 작성자 정보 추가
    User *author = userService_->getUserById(post->getUserId());
    if (author) {
        jsonObj.insert("userName", author->getName());
    }

    return jsonObj;
}

QJsonArray ProtocolController::vectorToJsonArray(const QVector<Post>& posts)
{
    QJsonArray array;
    for (const Post& post : posts) {
        QJsonObject postObj = post.toJson();

        // 각 게시글에 작성자 이름 추가
        User *author = userService_->getUserById(post.getUserId());
        if (author) {
            postObj.insert("authorName", author->getName());
        }

        array.append(postObj);
    }
    return array;
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

QJsonObject ProtocolController::plantToJson(const Plant* plant)
{
    return plant ? plant->toJson() : QJsonObject();
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

QJsonArray ProtocolController::vectorToJsonArray(const QVector<Plant>& plants)
{
    QJsonArray array;
    for (const Plant& plant : plants) {
        array.append(plant.toJson());
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

QString ProtocolController::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
