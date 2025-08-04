#include "Order.h"

// getter
id_t Order::getId() const {
    return orderId;
}

id_t Order::getUserId() const {
    return userId;
}

QVector<id_t> Order::getItemIds() const {
    return orderItemIds;
}

QDateTime Order::getOrderDate() const {
    return orderDate;
}

// setter
void Order::setId(id_t id){
    orderId = id;
}

RaErrorCode Order::setItemIds(QVector<id_t> itemIds){
    orderItemIds = itemIds;
    return Ra_Success;
}

// json
QJsonObject Order::toJson() const {

    QJsonObject jsonObject;
    jsonObject.insert("orderId",orderId);
    jsonObject.insert("userId",userId);

    QJsonObject jsonOrderItemIds;
    for(int i=0; i<orderItemIds.size(); i++){
        QString tmpStr = QString("orderItemId%1").arg(i);
        jsonOrderItemIds.insert(tmpStr,orderItemIds[i]);
    }
    jsonObject.insert("orderItemIds", jsonOrderItemIds);
    jsonObject.insert("orderDate", orderDate.toString());

    return jsonObject;
}
RaErrorCode Order::fromJson(const QJsonObject& inputJson){
    orderId = inputJson.value("orderId").toInteger();
    userId = inputJson.value("userId").toInteger();
    orderDate.fromString(inputJson.value("orderDate").toString());

    QJsonObject jsonOrderItemIds = inputJson.value("orderItemIds").toObject();
    qint32 size = jsonOrderItemIds.size();
    for(int i=0; i<size; i++){
        QString tmpStr = QString("orderItemId%1").arg(i);
        orderItemIds.append(jsonOrderItemIds.value(tmpStr).toInteger());
    }


    return Ra_Success;
}
