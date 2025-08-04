#include "ChatRoomJsonRepo.h"

ChatRoomJsonRepo* ChatRoomJsonRepo::instance_ = nullptr;

ChatRoomJsonRepo::ChatRoomJsonRepo() : JsonRepo<ChatRoom>(){
    // 필요한 초기화 코드가 있다면 여기에 추가
}

ChatRoomJsonRepo* ChatRoomJsonRepo::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new ChatRoomJsonRepo();
    }
    return instance_;
}

void ChatRoomJsonRepo::destroyInstance() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void ChatRoomJsonRepo::debugPrint(){
    for (ChatRoom* room : vectorData_) {
        id_t roomId = room->getId();
        QString roomName = room->getRoomName();
        qDebug() << QString("roomId(%1), roomName(%2)").arg(roomId).arg(roomName);
    }
}
