#ifndef ORDERITEM_H
#define ORDERITEM_H

#include "models/entity/Entity.h"

class OrderItem : Entity{
private:
    id_t itemId;
    id_t productId;
    qint32 quantity;
    qreal unitPrice;  // 구매 당시 가격
public:
    OrderItem() = default;
    OrderItem(id_t id, id_t pid, qint32 quantity, qreal unitPrice)
        : itemId(id), productId(pid), quantity(quantity), unitPrice(unitPrice) {}

    // getter
    id_t getId() const  override;
    id_t getProductId() const ;
    qint32 getQuantity() const ;
    qreal getUnitPrice() const ;
    qreal getTotlaPrice() const ;

    // setter
    void setId(id_t id) override;
    RaErrorCode setQuantity(qint32 num);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;

};

#endif // ORDERITEM_H
