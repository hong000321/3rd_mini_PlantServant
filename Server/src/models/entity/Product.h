#ifndef PRODUCT_H
#define PRODUCT_H

#include "models/entity/Entity.h"
#include "models/repository/ChatRoomJsonRepo.h"

class Product : Entity{
private:
    id_t productId_;
    id_t roomId_ = -1;
    QString name_;
    QString category_;
    qreal price_;
    qint32 stock_;
    QString imageFileName_;  // 추가: 이미지 파일명
    ChatRoomJsonRepo *roomRepo_;

public:
    // setter : constructor
    Product(){
        roomRepo_ = ChatRoomJsonRepo::getInstance();
    };

    Product(QString name, QString category, qreal price, qint32 stock)
        : name_(name), category_(category), price_(price), stock_(stock) {
        roomRepo_ = ChatRoomJsonRepo::getInstance();
    }

    Product(QString name, QString category, qreal price, qint32 stock, QString imageFileName)
        : name_(name), category_(category), price_(price), stock_(stock), imageFileName_(imageFileName) {
        roomRepo_ = ChatRoomJsonRepo::getInstance();
    }

    // getter
    id_t getId() const  override;
    id_t getRoomId() const;
    QString getName() const ;
    QString getCategory() const ;
    qreal getPrice() const ;
    qint32 getStock() const ;
    QString getImageFileName() const;  // 추가

    // setter
    void setId(id_t id) override;
    RaErrorCode setName(QString inputName);
    RaErrorCode setPrice(qreal inputPrice);
    RaErrorCode setCategory(QString inputCategory);
    RaErrorCode setImageFileName(const QString &fileName);  // 추가

    // etc
    RaErrorCode increaseStock(quint32 num);
    RaErrorCode decreaseStock(quint32 num);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& json) override;

};

#endif // PRODUCT_H
