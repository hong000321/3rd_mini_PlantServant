#ifndef ORDER_H
#define ORDER_H

#include "models/entity/Entity.h"
#include "OrderItem.h"

#include <QDateTime>


class Order : Entity{
private:
    id_t orderId_;
    id_t userId_;
    QVector<id_t> orderItemIds_;
    QDateTime orderDate_;

public:
    Order() = default;
    Order(id_t id, id_t userId, QVector<id_t> orderItemIds, QDateTime orderDate)
        : orderId_(id), userId_(userId), orderItemIds_(orderItemIds), orderDate_(orderDate) {}

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
