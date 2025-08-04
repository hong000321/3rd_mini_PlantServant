#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QJsonObject>
#define id_t qint64
class Chat : public QObject
{
    Q_OBJECT

private:
    id_t senderId_;
    QString senderName_;
    QString chatStr_;

public:
    explicit Chat(QObject *parent = nullptr);

    // constructor
    Chat(const id_t& senderId, const QString& senderName, const QString& chatStr)
        : senderId_(senderId), senderName_(senderName), chatStr_(chatStr){}

    // getter
    id_t getSenderId() const;
    QString getSenderName() const;
    QString getChatStr() const;

    //setter
    bool setSenderId(const id_t& senderId);
    bool setSenderName(const QString& senderName);
    bool setChatStr(const QString& chatStr);

    //toJson, fromJson
    QJsonObject toJson() const;
    static Chat fromJson(const QJsonObject& chatObj);

signals:
};

#endif // CHAT_H
