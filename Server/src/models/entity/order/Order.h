#ifndef ORDER_H
#define ORDER_H

#include "models/entity/Entity.h"
#include "OrderItem.h"

#include <QDateTime>


class Order : Entity{
private:
    id_t orderId;
    id_t userId;
    QVector<id_t> orderItemIds;
    QDateTime orderDate;

public:
    Order() = default;
    Order(id_t id, id_t userId, QVector<id_t> orderItemIds, QDateTime orderDate)
        : orderId(id), userId(userId), orderItemIds(orderItemIds), orderDate(orderDate) {}

    // getter
    id_t getId() const  override;
    id_t getUserId() const ;
    QVector<id_t> getItemIds() const ;
    QDateTime getOrderDate() const ;

    // setter
    void setId(id_t id) override;
    RaErrorCode setItemIds(QVector<id_t> itemIds);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;

};

#endif // ORDER_H
