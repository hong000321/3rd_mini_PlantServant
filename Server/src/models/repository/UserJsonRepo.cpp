#include "UserJsonRepo.h"

UserJsonRepo* UserJsonRepo::instance = nullptr;

UserJsonRepo::UserJsonRepo() : JsonRepo<User>(){
}

UserJsonRepo* UserJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new UserJsonRepo();
    }
    return instance;
}

void UserJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

User* UserJsonRepo::findUserByName(const QString& name){
    for (User* user : vectorData) {
        if (user->getName() == name) {
            return user;
        }
    }
    return nullptr;
}

User* UserJsonRepo::findUserByStrId(const QString& strId){
    for (User* user : vectorData) {
        if (user->getstrId() == strId) {
            return user;
        }
    }
    return nullptr;
}

QVector<User> UserJsonRepo::getUsersByPermissionLevel(PermissionLevel level){
    QVector<User> result;
    for (User* user : vectorData) {
        if (user->verifyLevel(level)) {
            result.append(*user);
        }
    }
    return result;
}

QVector<User> UserJsonRepo::getConnectedUsers(){
    QVector<User> result;
    for (User* user : vectorData) {
        if (user->isConnected()) {
            result.append(*user);
        }
    }
    return result;
}
