#ifndef POST_H
#define POST_H

#include "models/entity/Entity.h"
#include <QDateTime>

class Post : public Entity
{
private:
    id_t postId_;
    QString title_;
    QString content_;
    QString imagePath_;
    QString imageBase64_;
    id_t userId_;
    QString userName_;
    QDateTime createdAt_;
    QDateTime updatedAt_;

public:
    Post() = default;
    Post(const QString& title, const QString& content, id_t userId, QString imagePath)
        : title_(title), content_(content), imagePath_(imagePath), userId_(userId){
        postId_ = -1;
        createdAt_ = QDateTime::currentDateTime();
        updatedAt_ = QDateTime::currentDateTime();
    }

    Post(id_t id, const QString& title, const QString& content, id_t userId,
         const QDateTime& createdAt, const QDateTime& updatedAt)
        : postId_(id), title_(title), content_(content), userId_(userId),
        createdAt_(createdAt), updatedAt_(updatedAt) {}

    // getter
    id_t getId() const override;
    QString getTitle() const;
    QString getContent() const;
    QString getImagePath() const;
    QString getUserName() const;
    id_t getUserId() const;
    QDateTime getCreatedAt() const;
    QDateTime getUpdatedAt() const;

    // setter
    void setId(id_t id) override;
    RaErrorCode setTitle(const QString& title);
    RaErrorCode setContent(const QString& content);
    RaErrorCode setImagePath(const QString& imagePath);
    RaErrorCode setImageBase64(const QString& imagePath);
    RaErrorCode setUserName(const QString& userName);
    RaErrorCode setUserId(id_t userId);
    RaErrorCode updateContent(const QString& newTitle, const QString& newContent);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;
};

#endif // POST_H
