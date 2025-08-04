#include "OrderItem.h"


// getter
id_t OrderItem::getId() const {
    return itemId_;
}

id_t OrderItem::getProductId() const {
    return productId_;
}

qint32 OrderItem::getQuantity() const {
    return quantity_;
}

qreal OrderItem::getUnitPrice() const {
    return unitPrice_;
}

qreal OrderItem::getTotlaPrice() const {
    return quantity_*unitPrice_;
}

// setter
void OrderItem::setId(id_t id){
    itemId_ = id;
}

RaErrorCode OrderItem::setQuantity(qint32 num){
    if(num<0){
        return Ra_Domain_Unkown_Error;
    }
    quantity_ = num;
    return Ra_Success;
}

QJsonObject OrderItem::toJson() const {
    QJsonObject jsonObject;
    jsonObject.insert("itemId",itemId_);
    jsonObject.insert("productId",productId_);
    jsonObject.insert("quantity",quantity_);
    jsonObject.insert("unitPrice",unitPrice_);

    return jsonObject;
}

RaErrorCode OrderItem::fromJson(const QJsonObject& inputJson){
    itemId_ = inputJson.value("itemId").toInteger();
    productId_ = inputJson.value("productId").toInteger();
    quantity_ = inputJson.value("quantity").toInt();
    unitPrice_ = inputJson.value("unitPrice").toDouble();

    return Ra_Success;
}
