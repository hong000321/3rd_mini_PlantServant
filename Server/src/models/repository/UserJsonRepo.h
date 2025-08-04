#ifndef USERJSONREPO_H
#define USERJSONREPO_H

#include "JsonRepo.h"
#include "models/Entity/user/User.h"

class UserJsonRepo : public JsonRepo<User> {
private:
    static UserJsonRepo* instance;
    UserJsonRepo();
    UserJsonRepo(const UserJsonRepo&) = delete;
    UserJsonRepo& operator=(const UserJsonRepo&) = delete;
    UserJsonRepo(UserJsonRepo&&) = delete;
    UserJsonRepo& operator=(UserJsonRepo&&) = delete;

public:
    static UserJsonRepo* getInstance();
    ~UserJsonRepo() = default;
    static void destroyInstance();

    User* findUserByName(const QString& name);
    User* findUserByStrId(const QString& strId);
    QVector<User> getUsersByPermissionLevel(PermissionLevel level);
    QVector<User> getConnectedUsers();
};

#endif // USERJSONREPO_H
