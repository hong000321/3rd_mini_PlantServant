#ifndef USER_H
#define USER_H

#include <QObject>
#define id_t qint64

class User : public QObject
{
    Q_OBJECT
private:
    QString userName_;
    QString userId_;
    QString pw_;

public:
    explicit User(QObject *parent = nullptr);

    User (const QString& name, const QString& strId, const QString& pw)
        : userName_(name), userId_(strId), pw_(pw){}

    //getter
    QString getUserName() const;
    QString getUserId() const;
    QString getUserPw() const;

    //setter
    bool setUserName(const QString& name);
    bool setUserId(const QString& id);
    bool setUserPw(const QString& pw);

    //toJson, fromJson
    QJsonObject toJson() const;
    static User fromJson (const QJsonObject& userObj);

signals:
};

#endif // USER_H
