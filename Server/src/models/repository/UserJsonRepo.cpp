#include "UserJsonRepo.h"

UserJsonRepo* UserJsonRepo::instance_ = nullptr;

UserJsonRepo::UserJsonRepo() : JsonRepo<User>(){
}

UserJsonRepo* UserJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new UserJsonRepo();
    }
    return instance_;
}

void UserJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

User* UserJsonRepo::findUserByName(const QString& name){
    for (User* user : vectorData_) {
        if (user->getName() == name) {
            return user;
        }
    }
    return nullptr;
}

User* UserJsonRepo::findUserByStrId(const QString& strId){
    for (User* user : vectorData_) {
        if (user->getstrId() == strId) {
            return user;
        }
    }
    return nullptr;
}

QVector<User> UserJsonRepo::getUsersByPermissionLevel(PermissionLevel level){
    QVector<User> result;
    for (User* user : vectorData_) {
        if (user->verifyLevel(level)) {
            result.append(*user);
        }
    }
    return result;
}

QVector<User> UserJsonRepo::getConnectedUsers(){
    QVector<User> result;
    for (User* user : vectorData_) {
        if (user->isConnected()) {
            result.append(*user);
        }
    }
    return result;
}
