#ifndef USER_H
#define USER_H

#include "models/entity/Entity.h"
enum PermissionLevel {UNKOWN_LEVEL, ADMIN_LEVEL, USER_LEVEL };

class User : Entity{
private:
    id_t userId_;
    id_t plantId_;
    QString strId_;
    QString password_;
    QString name_;
    QString email_;
    QString address_;
    PermissionLevel level_; // : 0 관리자, 1 고객
    bool boolConnected_; // : 현재 접속 유무
public:
    User() = default;
    User(QString strId, QString password, QString name, QString email, QString address, PermissionLevel level, bool boolConnected = false)
        : strId_(strId), password_(password), name_(name), email_(email), address_(address), level_(level), boolConnected_(boolConnected) {userId_=-1;}

    // getter
    id_t getId() const override;
    id_t getPlantId() const ;
    QString getstrId() const ;
    QString getName() const ;
    QString getEmail() const ;
    QString getAddress() const ;
    bool isConnected() const ;

    // setter
    void setId(id_t id) override;
    void setPlantId(id_t id);
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
