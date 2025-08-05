// PostService.h
#ifndef POSTSERVICE_H
#define POSTSERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include "controllers/ClientSocket.h"
#include "models/entity/Post.h"
#include "QPixmap"

#define id_t qint64

class PostService : public QObject {
    Q_OBJECT

public:
    explicit PostService(ClientSocket* socket, QObject* parent = nullptr);

    void createPost(const Post& post);
    void requestPostList();
    void requestPost(id_t postId);

signals:
    void postCreated(const QJsonObject& post);
    void postListReceived(const QJsonArray& posts);
    void postReceived(const Post& post);

private slots:
    void onPostCreated(const QJsonObject& post);
    void onPostListReceived(const QJsonArray& posts);
    void onPostReceived(const QJsonObject& post);


private:
    ClientSocket* socket_;
};

#endif // POSTSERVICE_H
