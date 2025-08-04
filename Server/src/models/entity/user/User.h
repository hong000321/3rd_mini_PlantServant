#ifndef USER_H
#define USER_H

#include "models/entity/Entity.h"
enum PermissionLevel {UNKOWN_LEVEL, ADMIN_LEVEL, USER_LEVEL };

class User : Entity{
private:
    id_t userId;
    QString strId;
    QString password;
    QString name;
    QString email;
    QString address;
    PermissionLevel level; // : 0 관리자, 1 고객
    bool boolConnected; // : 현재 접속 유무
public:
    User() = default;
    User(QString strId, QString password, QString name, QString email, QString address, PermissionLevel level, bool boolConnected = false)
        : strId(strId), password(password), name(name), email(email), address(address), level(level), boolConnected(boolConnected) {userId=-1;}

    // getter
    id_t getId() const override;
    QString getstrId() const ;
    QString getName() const ;
    QString getEmail() const ;
    QString getAddress() const ;
    bool isConnected() const ;

    // setter
    void setId(id_t id) override;
    RaErrorCode connect();
    RaErrorCode disconnect();

    //etc
    bool verifystrId(QString inputstrId);
    bool verifyPassword(QString inputPass);
    bool verifyLevel(PermissionLevel inputLevel);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;
};

#endif // USER_H
