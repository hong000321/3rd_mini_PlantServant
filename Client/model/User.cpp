#include "User.h"
#include <QJsonObject>

User::User(QObject *parent)
    :  QObject(parent), userName_(""), userId_(""), pw_("") {}

// getter
QString User::getUserName() const {
    return userName_;
}

QString User::getUserId() const {
    return userId_;
}

QString User::getUserPw() const {
    return pw_;
}

// setter
bool User::setUserName(const QString &name) {
    if (name.isEmpty())
        return false;
    userName_ = name;
    return true;
}

bool User::setUserId(const QString &id) {
    if (id.isEmpty())
        return false;
    userId_ = id;
    return true;
}

bool User::setUserPw(const QString &pw) {
    if (pw.isEmpty())
        return false;
    pw_ = pw;
    return true;
}

// JSON 직렬화
QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["userId"] = userId_;
    obj["userName"] = userName_;
    obj["pw"] = pw_;
    return obj;
}

// JSON 역직렬화
User User::fromJson(const QJsonObject &userObj) {
    QString name = userObj["userName"].toString();
    QString id = userObj["userId"].toString();
    QString pw = userObj["pw"].toString();
    return User(name, id, pw);
}

