#ifndef CHATROOMJSONREPO_H
#define CHATROOMJSONREPO_H
#include "JsonRepo.h"
#include "models/entity/chatlog/ChatRoom.h"
#include "models/entity/chatlog/ChatUnit.h"
#include "utils/CustomErrorCodes.h"


class ChatRoomJsonRepo : public JsonRepo<ChatRoom> {\
protected:
    using JsonRepo::vectorData_;
    using JsonRepo::filepath_;
private:
    static ChatRoomJsonRepo* instance_;  // 싱글톤 인스턴스
    ChatRoomJsonRepo();
    ChatRoomJsonRepo(const ChatRoomJsonRepo&) = delete;
    ChatRoomJsonRepo& operator=(const ChatRoomJsonRepo&) = delete;
    ChatRoomJsonRepo(ChatRoomJsonRepo&&) = delete;
    ChatRoomJsonRepo& operator=(ChatRoomJsonRepo&&) = delete;
public:
    static ChatRoomJsonRepo* getInstance();
    ~ChatRoomJsonRepo() = default;
    static void destroyInstance();
    void debugPrint();
};

#endif // CHATROOMJSONREPO_H
