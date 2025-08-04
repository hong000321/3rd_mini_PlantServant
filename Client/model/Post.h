#ifndef POST_H
#define POST_H

#include <QObject>
#include <QJsonObject>

#define id_t qint64

class Post : public QObject
{
    Q_OBJECT

private:
    id_t postId_;
    id_t userId_;
    QString userName_;
    QString title_;
    QString content_;

public:
    explicit Post(QObject *parent = nullptr);

    Post (id_t postId, id_t userdId, const QString& userName, const QString& title, const QString& content)
        : postId_(postId), userId_(userdId), userName_(userName), title_(title), content_(content){}

    // getter
    id_t getPostId() const;
    id_t getUserId() const;
    QString getUserName() const;
    QString getTitle() const;
    QString getContent() const;

    // setter
    bool setPostId(const id_t& postId);
    bool setUserId(const id_t& userId);
    bool setUserName(const QString& userName);
    bool setTitle(const QString& title);
    bool setContent(const QString& content);

    QJsonObject toJson() const;
    static Post fromJson (const QJsonObject& postObj);

signals:
};

#endif // POST_H
