#ifndef PRODUCT_H
#define PRODUCT_H

#include "models/entity/Entity.h"
#include "models/repository/ChatRoomJsonRepo.h"

class Product : Entity{
private:
    id_t productId;
    id_t roomId = -1;
    QString name;
    QString category;
    qreal price;
    qint32 stock;
    QString imageFileName;  // 추가: 이미지 파일명
    ChatRoomJsonRepo *roomRepo;

public:
    // setter : constructor
    Product(){
        roomRepo = ChatRoomJsonRepo::getInstance();
    };

    Product(QString name, QString category, qreal price, qint32 stock)
        : name(name), category(category), price(price), stock(stock) {
        roomRepo = ChatRoomJsonRepo::getInstance();
    }

    Product(QString name, QString category, qreal price, qint32 stock, QString imageFileName)
        : name(name), category(category), price(price), stock(stock), imageFileName(imageFileName) {
        roomRepo = ChatRoomJsonRepo::getInstance();
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
