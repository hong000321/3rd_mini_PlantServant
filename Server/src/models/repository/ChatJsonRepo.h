
#ifndef CHATJSONREPO_H
#define CHATJSONREPO_H
#include "JsonRepo.h"
#include "models/entity/ChatUnit.h"

class ChatJsonRepo : public JsonRepo<ChatUnit> {
private:
    static ChatJsonRepo* instance_;  // 싱글톤 인스턴스
    ChatJsonRepo();
    ChatJsonRepo(const ChatJsonRepo&) = delete;
    ChatJsonRepo& operator=(const ChatJsonRepo&) = delete;
    ChatJsonRepo(ChatJsonRepo&&) = delete;
    ChatJsonRepo& operator=(ChatJsonRepo&&) = delete;

public:
    static ChatJsonRepo* getInstance();
    ~ChatJsonRepo() = default;
    static void destroyInstance();
};

#endif // CHATJSONREPO_H
