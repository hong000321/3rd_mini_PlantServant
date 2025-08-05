// PostService.cpp
#include "PostService.h"
#include "entity/Post.h"

PostService::PostService(ClientSocket* socket, QObject* parent)
    : QObject(parent), socket_(socket)
{
    connect(socket_, &ClientSocket::postCreated, this, &PostService::onPostCreated);
    connect(socket_, &ClientSocket::postListReceived, this, &PostService::onPostListReceived);
    connect(socket_, &ClientSocket::postReceived, this, &PostService::onPostReceived);
}

// void PostService::createPost(const QString& title, const QString& content, id_t userId) {
//     QJsonObject params;
//     params["title"] = title;
//     params["content"] = content;
//     params["userId"] = static_cast<int>(userId);

//     QJsonObject message = socket_->createCommandMessage("create", "post", params);
//     socket_->sendMessage(message);
// }

void PostService::createPost(const Post& post)
{
    QJsonObject params = post.toJson();  // Post 객체 → JSON
    qDebug() << "📤 서버에 보낼 JSON:" << QJsonDocument(params).toJson();
    QJsonObject message = socket_->createCommandMessage("create", "post", params);
    socket_->sendMessage(message);
}

void PostService::requestPostList() {
    QJsonObject message = socket_->createCommandMessage("list", "post");
    socket_->sendMessage(message);
}

void PostService::requestPost(id_t postId) {
    QJsonObject params;
    params["postId"] = static_cast<int>(postId);
    QJsonObject message = socket_->createCommandMessage("get", "post", params);
    socket_->sendMessage(message);
    qDebug() << "📤 GET 요청 전송:" << QJsonDocument(message).toJson();
}

void PostService::onPostCreated(const QJsonObject& post) {
    emit postCreated(post);
}

void PostService::onPostListReceived(const QJsonArray& posts) {
    emit postListReceived(posts);
}

void PostService::onPostReceived(const QJsonObject& postJson) {
    qDebug() << "📥 서버에서 받은 post JSON:" << QJsonDocument(postJson).toJson();
    Post post;
    post.fromJson(postJson);
    emit postReceived(post);
}

