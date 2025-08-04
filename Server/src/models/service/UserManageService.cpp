#include "UserManageService.h"
#include <QDebug>

UserManageService* UserManageService::instance_ = nullptr;

UserManageService::UserManageService(QObject *parent)
    : QObject(parent)
{
    userRepo_ = UserJsonRepo::getInstance();
}

UserManageService* UserManageService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new UserManageService();
    }
    return instance_;
}

void UserManageService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode UserManageService::createUser(const User& user)
{
    try {
        // 중복 검사
        if (userRepo_->findUserByStrId(user.getstrId()) != nullptr) {
            qDebug() << "User already exists with strId:" << user.getstrId();
            return Ra_Domain_Unkown_Error;
        }

        id_t userId = userRepo_->insert(user);
        if (userId>=0) {
            emit userCreated(userId);
            qDebug() << "User created successfully, ID:" << userId;
            return Ra_Success;
        }
    } catch (...) {
        qDebug() << "Exception in createUser";
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode UserManageService::updateUser(const User& user)
{
    try {
        const User* existingUser = userRepo_->getObjPtrById(user.getId());
        if (existingUser == nullptr) {
            qDebug() << "User not found for update, ID:" << user.getId();
            return Ra_Domain_Unkown_Error;
        }

        if (userRepo_->update(user)) {
            emit userUpdated(user.getId());
            qDebug() << "User updated successfully, ID:" << user.getId();
            return Ra_Success;
        }
    } catch (...) {
        qDebug() << "Exception in updateUser";
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode UserManageService::deleteUser(id_t userId)
{
    try {
        if (userRepo_->removeById(userId)) {
            emit userDeleted(userId);
            qDebug() << "User deleted successfully, ID:" << userId;
            return Ra_Success;
        }
    } catch (...) {
        qDebug() << "Exception in deleteUser";
    }

    return Ra_Domain_Unkown_Error;
}

User* UserManageService::getUserById(id_t userId)
{
    try {
        return userRepo_->getObjPtrById(userId);
    } catch (...) {
        qDebug() << "Exception in getUserById";
        return nullptr;
    }
}

User* UserManageService::getUserByStrId(const QString& strId)
{
    try {
        return userRepo_->findUserByStrId(strId);
    } catch (...) {
        qDebug() << "Exception in getUserByStrId";
        return nullptr;
    }
}

User* UserManageService::getUserByName(const QString& name)
{
    try {
        return userRepo_->findUserByName(name);
    } catch (...) {
        qDebug() << "Exception in getUserByName";
        return nullptr;
    }
}

RaErrorCode UserManageService::authenticateUser(const QString& strId, const QString& password, User& outUser)
{
    try {
        const User* user = userRepo_->findUserByStrId(strId);
        if (user == nullptr) {
            qDebug() << "User not found for authentication, strId:" << strId;
            return Ra_Domain_Unkown_Error;
        }

        // const 객체이므로 복사본을 만들어서 검증
        User tempUser = *user;
        if (!tempUser.verifyPassword(password)) {
            qDebug() << "Password verification failed for user:" << strId;
            return Ra_Domain_Unkown_Error;
        }

        outUser = *user;
        qDebug() << "User authenticated successfully:" << strId;
        return Ra_Success;
    } catch (...) {
        qDebug() << "Exception in authenticateUser";
        return Ra_Domain_Unkown_Error;
    }
}

bool UserManageService::verifyUserPermission(id_t userId, PermissionLevel requiredLevel)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        if (user == nullptr) {
            qDebug() << "User not found for permission check, ID:" << userId;
            return false;
        }

        // const 객체이므로 복사본을 만들어서 검증
        User tempUser = *user;
        return tempUser.verifyLevel(requiredLevel);
    } catch (...) {
        qDebug() << "Exception in verifyUserPermission";
        return false;
    }
}

RaErrorCode UserManageService::changePassword(id_t userId, const QString& oldPassword, const QString& newPassword)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        if (user == nullptr) {
            qDebug() << "User not found for password change, ID:" << userId;
            return Ra_Domain_Unkown_Error;
        }

        User tempUser = *user;
        if (!tempUser.verifyPassword(oldPassword)) {
            qDebug() << "Old password verification failed for user ID:" << userId;
            return Ra_Domain_Unkown_Error;
        }

        User updatedUser = *user;
        // User 클래스에 setPassword 메서드가 없으므로 JSON을 통해 업데이트
        QJsonObject userJson = updatedUser.toJson();
        userJson["password"] = newPassword;

        if (updatedUser.fromJson(userJson) == Ra_Success) {
            if (userRepo_->update(updatedUser)) {
                qDebug() << "Password changed successfully for user ID:" << userId;
                return Ra_Success;
            }
        }
    } catch (...) {
        qDebug() << "Exception in changePassword";
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode UserManageService::connectUser(id_t userId)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        if (user == nullptr) {
            qDebug() << "User not found for connection, ID:" << userId;
            return Ra_Domain_Unkown_Error;
        }

        User updatedUser = *user;
        RaErrorCode result = updatedUser.connect();
        if (result == Ra_Success) {
            if (userRepo_->update(updatedUser)) {
                emit userConnected(userId);
                qDebug() << "User connected successfully, ID:" << userId;
                return Ra_Success;
            }
        }
    } catch (...) {
        qDebug() << "Exception in connectUser";
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode UserManageService::disconnectUser(id_t userId)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        if (user == nullptr) {
            qDebug() << "User not found for disconnection, ID:" << userId;
            return Ra_Domain_Unkown_Error;
        }

        User updatedUser = *user;
        RaErrorCode result = updatedUser.disconnect();
        if (result == Ra_Success) {
            if (userRepo_->update(updatedUser)) {
                emit userDisconnected(userId);
                qDebug() << "User disconnected successfully, ID:" << userId;
                return Ra_Success;
            }
        }
    } catch (...) {
        qDebug() << "Exception in disconnectUser";
    }

    return Ra_Domain_Unkown_Error;
}

bool UserManageService::isUserConnected(id_t userId)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        return user != nullptr && user->isConnected();
    } catch (...) {
        qDebug() << "Exception in isUserConnected";
        return false;
    }
}

QVector<User> UserManageService::getAllUsers()
{
    try {
        return userRepo_->getAllObjects();
    } catch (...) {
        qDebug() << "Exception in getAllUsers";
        return QVector<User>();
    }
}

bool UserManageService::loadUsers(const QString& filePath)
{
    try {
        bool result = userRepo_->loadDataFromFile(filePath);
        if (result) {
            qDebug() << "Users loaded successfully from:" << filePath;
            qDebug() << "Total users loaded:" << userRepo_->getSize();
        } else {
            qDebug() << "Failed to load users from:" << filePath;
        }
        return result;
    } catch (...) {
        qDebug() << "Exception in loadUsers";
        return false;
    }
}

bool UserManageService::saveUsers()
{
    try {
        bool result = userRepo_->saveToFile();
        if (result) {
            qDebug() << "Users saved successfully. Total users:" << userRepo_->getSize();
        } else {
            qDebug() << "Failed to save users";
        }
        return result;
    } catch (...) {
        qDebug() << "Exception in saveUsers";
        return false;
    }
}

int UserManageService::getUserCount()
{
    try {
        return userRepo_->getSize();
    } catch (...) {
        qDebug() << "Exception in getUserCount";
        return 0;
    }
}

// UserJsonRepo에서 제거된 메서드들은 여기서 직접 구현
QVector<User> UserManageService::getUsersByPermissionLevel(PermissionLevel level)
{
    QVector<User> result;
    try {
        QVector<User> allUsers = userRepo_->getAllObjects();

        for (const User& user : allUsers) {
            User tempUser = user; // const 문제 해결을 위한 복사
            if (tempUser.verifyLevel(level)) {
                result.append(user);
            }
        }

        qDebug() << "Found" << result.size() << "users with permission level:" << static_cast<int>(level);
    } catch (...) {
        qDebug() << "Exception in getUsersByPermissionLevel";
    }

    return result;
}

QVector<User> UserManageService::getConnectedUsers()
{
    QVector<User> result;
    try {
        QVector<User> allUsers = userRepo_->getAllObjects();

        for (const User& user : allUsers) {
            if (user.isConnected()) {
                result.append(user);
            }
        }

        qDebug() << "Found" << result.size() << "connected users";
    } catch (...) {
        qDebug() << "Exception in getConnectedUsers";
    }

    return result;
}

QVector<User> UserManageService::searchUsersByName(const QString& namePattern)
{
    QVector<User> result;
    try {
        QVector<User> allUsers = userRepo_->getAllObjects();

        for (const User& user : allUsers) {
            if (user.getName().contains(namePattern, Qt::CaseInsensitive)) {
                result.append(user);
            }
        }

        qDebug() << "Found" << result.size() << "users matching name pattern:" << namePattern;
    } catch (...) {
        qDebug() << "Exception in searchUsersByName";
    }

    return result;
}

bool UserManageService::isUserEmailUnique(const QString& email, id_t excludeUserId)
{
    try {
        QVector<User> allUsers = userRepo_->getAllObjects();

        for (const User& user : allUsers) {
            if (user.getId() != excludeUserId && user.getEmail() == email) {
                return false;
            }
        }

        return true;
    } catch (...) {
        qDebug() << "Exception in isUserEmailUnique";
        return false;
    }
}

bool UserManageService::isUserStrIdUnique(const QString& strId, id_t excludeUserId)
{
    try {
        const User* existingUser = userRepo_->findUserByStrId(strId);
        return existingUser == nullptr || existingUser->getId() == excludeUserId;
    } catch (...) {
        qDebug() << "Exception in isUserStrIdUnique";
        return false;
    }
}

RaErrorCode UserManageService::updateUserProfile(id_t userId, const QString& name, const QString& email, const QString& address)
{
    try {
        const User* user = userRepo_->getObjPtrById(userId);
        if (user == nullptr) {
            qDebug() << "User not found for profile update, ID:" << userId;
            return Ra_Domain_Unkown_Error;
        }

        // 이메일 중복 검사
        if (!isUserEmailUnique(email, userId)) {
            qDebug() << "Email already exists:" << email;
            return Ra_Domain_Unkown_Error;
        }

        User updatedUser = *user;
        QJsonObject userJson = updatedUser.toJson();
        userJson["name"] = name;
        userJson["email"] = email;
        userJson["address"] = address;

        if (updatedUser.fromJson(userJson) == Ra_Success) {
            if (userRepo_->update(updatedUser)) {
                emit userUpdated(userId);
                qDebug() << "User profile updated successfully, ID:" << userId;
                return Ra_Success;
            }
        }
    } catch (...) {
        qDebug() << "Exception in updateUserProfile";
    }

    return Ra_Domain_Unkown_Error;
}
