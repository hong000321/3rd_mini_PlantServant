#include "Product.h"

// getter
id_t Product::getId() const {
    return productId;
}

id_t Product::getRoomId() const {
    ChatRoom *room = roomRepo->getObjPtrById(roomId);

    if(!room){
        return -1;
    }

    return roomId;
}

QString Product::getName() const {
    return name;
}

QString Product::getCategory() const {
    return category;
}

qreal Product::getPrice() const {
    return price;
}

qint32 Product::getStock() const {
    return stock;
}

QString Product::getImageFileName() const {
    return imageFileName;
}

// setter
void Product::setId(id_t id){
    productId = id;
}

RaErrorCode Product::setName(QString inputName){
    name = inputName;
    ChatRoom *room = roomRepo->getObjPtrById(roomId);
    if (room) {
        room->setChatRoomName(name);
    }
    return Ra_Success;
}

RaErrorCode Product::setPrice(qreal inputPrice){
    price = inputPrice;
    return Ra_Success;
}

RaErrorCode Product::setCategory(QString inputCategory){
    category = inputCategory;
    return Ra_Success;
}

RaErrorCode Product::setImageFileName(const QString &fileName){
    imageFileName = fileName;
    return Ra_Success;
}

// etc
RaErrorCode Product::increaseStock(quint32 num){
    stock+=num;
    return Ra_Success;
}

RaErrorCode Product::decreaseStock(quint32 num){
    if((stock-num)<0){
        return Ra_Domain_Unkown_Error;
    }
    stock-=num;
    return Ra_Success;
}

QJsonObject Product::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("productId", productId);
    jsonObject.insert("roomId", roomId);
    jsonObject.insert("name", name);
    jsonObject.insert("category", category);
    jsonObject.insert("price", price);
    jsonObject.insert("stock", stock);
    jsonObject.insert("imageFileName", imageFileName);  // 추가

    return jsonObject;
}

RaErrorCode Product::fromJson(const QJsonObject& inputJson){
    productId = inputJson.value("productId").toInteger();
    roomId = inputJson.value("roomId").toInteger();
    name = inputJson.value("name").toString();
    category = inputJson.value("category").toString();
    price = inputJson.value("price").toDouble();
    stock = inputJson.value("stock").toInt();
    imageFileName = inputJson.value("imageFileName").toString();  // 추가

    if(roomRepo->getObjPtrById(roomId) == nullptr){
        qDebug() << "roomId not Found " << roomId;
        roomId = roomRepo->insert(ChatRoom(name, QVector<id_t>(),QVector<id_t>()));
    }
    if(roomId < 0){
        qDebug() << "roomId is invalid " << roomId;
        roomId = roomRepo->insert(ChatRoom(name, QVector<id_t>(),QVector<id_t>()));
    }
    qDebug() << "roomId = " << roomId;

    return Ra_Success;
}
