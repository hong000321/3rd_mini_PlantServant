#ifndef USERMANAGESERVICE_H
#define USERMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "models/entity/User.h"
#include "models/repository/UserJsonRepo.h"
#include "utils/CustomErrorCodes.h"

class UserManageService : public QObject
{
    Q_OBJECT

private:
    static UserManageService* instance_;
    UserJsonRepo* userRepo_;

    UserManageService(QObject *parent = nullptr);
    UserManageService(const UserManageService&) = delete;
    UserManageService& operator=(const UserManageService&) = delete;

public:
    static UserManageService* getInstance();
    static void destroyInstance();
    ~UserManageService() = default;

    // 사용자 관리 메서드들 (수정된 시그니처)
    RaErrorCode createUser(const User& user);
    RaErrorCode updateUser(const User& user);
    RaErrorCode deleteUser(id_t userId);
    User* getUserById(id_t userId);
    User* getUserByStrId(const QString& strId);
    User* getUserByName(const QString& name);

    // 인증 관련 메서드들
    RaErrorCode authenticateUser(const QString& strId, const QString& password, User& outUser);
    bool verifyUserPermission(id_t userId, PermissionLevel requiredLevel);
    RaErrorCode changePassword(id_t userId, const QString& oldPassword, const QString& newPassword);

    // 연결 상태 관리
    RaErrorCode connectUser(id_t userId);
    RaErrorCode disconnectUser(id_t userId);
    bool isUserConnected(id_t userId);

    // 데이터 관리
    bool loadUsers(const QString& filePath);
    bool saveUsers();
    QVector<User> getAllUsers();
    int getUserCount();
    QVector<User> getUsersByPermissionLevel(PermissionLevel level);
    QVector<User> getConnectedUsers();
    QVector<User> searchUsersByName(const QString& namePattern);
    bool isUserEmailUnique(const QString& email, id_t excludeUserId = -1);
    bool isUserStrIdUnique(const QString& strId, id_t excludeUserId = -1);
    RaErrorCode updateUserProfile(id_t userId, const QString& name, const QString& email, const QString& address);

signals:
    void userCreated(id_t userId);
    void userUpdated(id_t userId);
    void userDeleted(id_t userId);
    void userConnected(id_t userId);
    void userDisconnected(id_t userId);
};

#endif // USERMANAGESERVICE_H
