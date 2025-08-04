#include "Order.h"

// getter
id_t Order::getId() const {
    return orderId_;
}

id_t Order::getUserId() const {
    return userId_;
}

QVector<id_t> Order::getItemIds() const {
    return orderItemIds_;
}

QDateTime Order::getOrderDate() const {
    return orderDate_;
}

// setter
void Order::setId(id_t id){
    orderId_ = id;
}

RaErrorCode Order::setItemIds(QVector<id_t> itemIds){
    orderItemIds_ = itemIds;
    return Ra_Success;
}

// json
QJsonObject Order::toJson() const {

    QJsonObject jsonObject;
    jsonObject.insert("orderId",orderId_);
    jsonObject.insert("userId",userId_);

    QJsonObject jsonOrderItemIds;
    for(int i=0; i<orderItemIds_.size(); i++){
        QString tmpStr = QString("orderItemId%1").arg(i);
        jsonOrderItemIds.insert(tmpStr,orderItemIds_[i]);
    }
    jsonObject.insert("orderItemIds", jsonOrderItemIds);
    jsonObject.insert("orderDate", orderDate_.toString());

    return jsonObject;
}

RaErrorCode Order::fromJson(const QJsonObject& inputJson){
    orderId_ = inputJson.value("orderId").toInteger();
    userId_ = inputJson.value("userId").toInteger();
    orderDate_.fromString(inputJson.value("orderDate").toString());

    QJsonObject jsonOrderItemIds = inputJson.value("orderItemIds").toObject();
    qint32 size = jsonOrderItemIds.size();
    for(int i=0; i<size; i++){
        QString tmpStr = QString("orderItemId%1").arg(i);
        orderItemIds_.append(jsonOrderItemIds.value(tmpStr).toInteger());
    }

    return Ra_Success;
}
