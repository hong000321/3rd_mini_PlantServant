#include "OrderItem.h"


// getter
id_t OrderItem::getId() const {
    return itemId;
}

id_t OrderItem::getProductId() const {
    return productId;
}

qint32 OrderItem::getQuantity() const {
    return quantity;
}

qreal OrderItem::getUnitPrice() const {
    return unitPrice;
}

qreal OrderItem::getTotlaPrice() const {
    return quantity*unitPrice;
}

// setter
void OrderItem::setId(id_t id){
    itemId = id;
}

RaErrorCode OrderItem::setQuantity(qint32 num){
    if(num<0){
        return Ra_Domain_Unkown_Error;
    }
    quantity = num;
    return Ra_Success;
}

QJsonObject OrderItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("itemId",itemId);
    jsonObject.insert("productId",productId);
    jsonObject.insert("quantity",quantity);
    jsonObject.insert("unitPrice",unitPrice);

    return jsonObject;
}

RaErrorCode OrderItem::fromJson(const QJsonObject& inputJson){
    itemId = inputJson.value("itemId").toInteger();
    productId = inputJson.value("productId").toInteger();
    quantity = inputJson.value("quantity").toInt();
    unitPrice = inputJson.value("unitPrice").toDouble();

    return Ra_Success;
}
