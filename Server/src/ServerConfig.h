#ifndef SERVERGLOBALH_H
#define SERVERGLOBALH_H

#include <QString>

#define DATA_FILE_PATH      "./../../data/"
#define LOG_FILE_PATH       "./../../log/"

#define USER_FILE_NAME        "user_data"
#define PRODUCT_FILE_NAME     "product_data"
#define ORDER_FILE_NAME       "order_data"
#define ORDERITEM_FILE_NAME   "orderItem_data"
#define CHAT_FILE_NAME        "chatUnit_data"
#define CHATROOM_FILE_NAME    "chatRoom_data"
#define JSON_FILE_EXTENSION   ".json"

#define LOG_FILE_NAME         "log_data"
#define LOG_FILE_EXTENSION    ".log"

class ServerConfig {
private:
    static ServerConfig* instance;  // 싱글톤 인스턴스
    ServerConfig();
    ServerConfig(const ServerConfig&) = delete;
    ServerConfig& operator=(const ServerConfig&) = delete;
    ServerConfig(ServerConfig&&) = delete;
    ServerConfig& operator=(ServerConfig&&) = delete;
    ~ServerConfig() = default;

public:
    static ServerConfig* getInstance();
    static void destroyInstance();

    QString userFilePath        = DATA_FILE_PATH USER_FILE_NAME      JSON_FILE_EXTENSION;
    QString productFilePath     = DATA_FILE_PATH PRODUCT_FILE_NAME   JSON_FILE_EXTENSION;
    QString orderFilePath       = DATA_FILE_PATH ORDER_FILE_NAME     JSON_FILE_EXTENSION;
    QString orderItemFilePath   = DATA_FILE_PATH ORDERITEM_FILE_NAME JSON_FILE_EXTENSION;
    QString chatFilePath        = DATA_FILE_PATH CHAT_FILE_NAME      JSON_FILE_EXTENSION;
    QString chatRoomFilePath    = DATA_FILE_PATH CHATROOM_FILE_NAME  JSON_FILE_EXTENSION;
    QString logFilePath         = LOG_FILE_PATH  LOG_FILE_NAME       JSON_FILE_EXTENSION;
};


#endif // SERVERGLOBALH_H
