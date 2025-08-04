#include "Post.h"

Post::Post(QObject *parent)
    : QObject(parent), postId_(0), userId_(0), userName_(""), title_(""), content_("") {}

// getter
id_t Post::getPostId() const {
    return postId_;
}

id_t Post::getUserId() const {
    return userId_;
}

QString Post::getUserName() const {
    return userName_;
}

QString Post::getTitle() const {
    return title_;
}

QString Post::getContent() const {
    return content_;
}

// setter
bool Post::setPostId(const id_t& postId) {
    if (postId < 0) return false;
    postId_ = postId;
    return true;
}

bool Post::setUserId(const id_t& userId) {
    if (userId < 0) return false;
    userId_ = userId;
    return true;
}

bool Post::setUserName(const QString& userName) {
    if (userName.trimmed().isEmpty()) return false;
    userName_ = userName;
    return true;
}

bool Post::setTitle(const QString& title) {
    if (title.trimmed().isEmpty()) return false;
    title_ = title;
    return true;
}

bool Post::setContent(const QString& content) {
    if (content.trimmed().isEmpty()) return false;
    content_ = content;
    return true;
}

// toJson 구현
QJsonObject Post::toJson() const {
    QJsonObject obj;
    obj["postId"] = static_cast<qint64>(postId_);
    obj["userId"] = static_cast<qint64>(userId_);
    obj["userName"] = userName_;
    obj["title"] = title_;
    obj["content"] = content_;
    return obj;
}

// fromJson 구현
Post Post::fromJson(const QJsonObject& postObj) {
    id_t postId = postObj["postId"].toVariant().toLongLong();
    id_t userId = postObj["userId"].toVariant().toLongLong();
    QString userName = postObj["userName"].toString();
    QString title = postObj["title"].toString();
    QString content = postObj["content"].toString();

    return Post(postId, userId, userName, title, content);
}
