#include "Product.h"

// getter
id_t Product::getId() const {
    return productId_;
}

id_t Product::getRoomId() const {
    ChatRoom *room = roomRepo_->getObjPtrById(roomId_);

    if(!room){
        return -1;
    }

    return roomId_;
}

QString Product::getName() const {
    return name_;
}

QString Product::getCategory() const {
    return category_;
}

qreal Product::getPrice() const {
    return price_;
}

qint32 Product::getStock() const {
    return stock_;
}

QString Product::getImageFileName() const {
    return imageFileName_;
}

// setter
void Product::setId(id_t id){
    productId_ = id;
}

RaErrorCode Product::setName(QString inputName){
    name_ = inputName;
    ChatRoom *room = roomRepo_->getObjPtrById(roomId_);
    if (room) {
        room->setChatRoomName(name_);
    }
    return Ra_Success;
}

RaErrorCode Product::setPrice(qreal inputPrice){
    price_ = inputPrice;
    return Ra_Success;
}

RaErrorCode Product::setCategory(QString inputCategory){
    category_ = inputCategory;
    return Ra_Success;
}

RaErrorCode Product::setImageFileName(const QString &fileName){
    imageFileName_ = fileName;
    return Ra_Success;
}

// etc
RaErrorCode Product::increaseStock(quint32 num){
    stock_+=num;
    return Ra_Success;
}

RaErrorCode Product::decreaseStock(quint32 num){
    if((stock_-num)<0){
        return Ra_Domain_Unkown_Error;
    }
    stock_-=num;
    return Ra_Success;
}

QJsonObject Product::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("productId", productId_);
    jsonObject.insert("roomId", roomId_);
    jsonObject.insert("name", name_);
    jsonObject.insert("category", category_);
    jsonObject.insert("price", price_);
    jsonObject.insert("stock", stock_);
    jsonObject.insert("imageFileName", imageFileName_);  // 추가

    return jsonObject;
}

RaErrorCode Product::fromJson(const QJsonObject& inputJson){
    productId_ = inputJson.value("productId").toInteger();
    roomId_ = inputJson.value("roomId").toInteger();
    name_ = inputJson.value("name").toString();
    category_ = inputJson.value("category").toString();
    price_ = inputJson.value("price").toDouble();
    stock_ = inputJson.value("stock").toInt();
    imageFileName_ = inputJson.value("imageFileName").toString();  // 추가

    if(roomRepo_->getObjPtrById(roomId_) == nullptr){
        qDebug() << "roomId not Found " << roomId_;
        roomId_ = roomRepo_->insert(ChatRoom(name_, QVector<id_t>(),QVector<id_t>()));
    }
    if(roomId_ < 0){
        qDebug() << "roomId is invalid " << roomId_;
        roomId_ = roomRepo_->insert(ChatRoom(name_, QVector<id_t>(),QVector<id_t>()));
    }
    qDebug() << "roomId = " << roomId_;

    return Ra_Success;
}
