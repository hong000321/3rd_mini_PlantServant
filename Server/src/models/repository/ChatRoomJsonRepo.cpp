#include "ChatRoomJsonRepo.h"

ChatRoomJsonRepo* ChatRoomJsonRepo::instance = nullptr;

ChatRoomJsonRepo::ChatRoomJsonRepo() : JsonRepo<ChatRoom>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ChatRoomJsonRepo* ChatRoomJsonRepo::getInstance() {
    if (instance == nullptr) {
        instance = new ChatRoomJsonRepo();
    }
    return instance;
}

void ChatRoomJsonRepo::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void ChatRoomJsonRepo::debugPrint(){
    for (ChatRoom* room : vectorData) {
        id_t roomId = room->getId();
        QString roomName = room->getRoomName();
        qDebug() << QString("roomId(%1), roomName(%2)").arg(roomId).arg(roomName);
    }
}
