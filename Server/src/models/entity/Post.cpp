#include "Post.h"

// getter
id_t Post::getId() const {
    return postId_;
}

QString Post::getTitle() const {
    return title_;
}

QString Post::getContent() const {
    return content_;
}

QString Post::getImagePath() const {
    return imagePath_;
}

id_t Post::getUserId() const {
    return userId_;
}

QDateTime Post::getCreatedAt() const {
    return createdAt_;
}

QDateTime Post::getUpdatedAt() const {
    return updatedAt_;
}

// setter
void Post::setId(id_t id) {
    postId_ = id;
}

RaErrorCode Post::setTitle(const QString& title) {
    if (title.isEmpty()) {
        return Ra_Domain_Unkown_Error;
    }
    title_ = title;
    updatedAt_ = QDateTime::currentDateTime();
    return Ra_Success;
}

RaErrorCode Post::setContent(const QString& content) {
    if (content.isEmpty()) {
        return Ra_Domain_Unkown_Error;
    }
    content_ = content;
    updatedAt_ = QDateTime::currentDateTime();
    return Ra_Success;
}

RaErrorCode Post::setImagePath(const QString& imagePath) {
    if (imagePath.isEmpty()) {
        return Ra_Domain_Unkown_Error;
    }
    imagePath_ = imagePath;
    updatedAt_ = QDateTime::currentDateTime();
    return Ra_Success;
}

RaErrorCode Post::setImageBase64(const QString& imageBase64){
    imageBase64_ = imageBase64;
    return Ra_Success;
}

RaErrorCode Post::setUserId(id_t userId) {
    if (userId < 0) {
        return Ra_Domain_Unkown_Error;
    }
    userId_ = userId;
    return Ra_Success;
}

RaErrorCode Post::updateContent(const QString& newTitle, const QString& newContent) {
    if (newTitle.isEmpty() || newContent.isEmpty()) {
        return Ra_Domain_Unkown_Error;
    }
    title_ = newTitle;
    content_ = newContent;
    updatedAt_ = QDateTime::currentDateTime();
    return Ra_Success;
}

// json
QJsonObject Post::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("postId", postId_);
    jsonObject.insert("title", title_);
    jsonObject.insert("content", content_);
    jsonObject.insert("imagePath", imagePath_);
    jsonObject.insert("userId", userId_);
    jsonObject.insert("createdAt", createdAt_.toString(Qt::ISODate));
    jsonObject.insert("updatedAt", updatedAt_.toString(Qt::ISODate));

    return jsonObject;
}

RaErrorCode Post::fromJson(const QJsonObject& inputJson) {
    postId_ = inputJson.value("postId").toInteger();
    title_ = inputJson.value("title").toString();
    content_ = inputJson.value("content").toString();
    imagePath_ = inputJson.value("imagePath").toString();
    userId_ = inputJson.value("userId").toInteger();

    QString createdAtStr = inputJson.value("createdAt").toString();
    QString updatedAtStr = inputJson.value("updatedAt").toString();

    if (!createdAtStr.isEmpty()) {
        createdAt_ = QDateTime::fromString(createdAtStr, Qt::ISODate);
    } else {
        createdAt_ = QDateTime::currentDateTime();
    }

    if (!updatedAtStr.isEmpty()) {
        updatedAt_ = QDateTime::fromString(updatedAtStr, Qt::ISODate);
    } else {
        updatedAt_ = QDateTime::currentDateTime();
    }

    return Ra_Success;
}
