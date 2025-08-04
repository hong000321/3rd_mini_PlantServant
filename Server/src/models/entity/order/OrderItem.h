#ifndef ORDERITEM_H
#define ORDERITEM_H

#include "models/entity/Entity.h"

class OrderItem : Entity{
private:
    id_t itemId_;
    id_t productId_;
    qint32 quantity_;
    qreal unitPrice_;  // 구매 당시 가격
public:
    OrderItem() = default;
    OrderItem(id_t id, id_t pid, qint32 quantity, qreal unitPrice)
        : itemId_(id), productId_(pid), quantity_(quantity), unitPrice_(unitPrice) {}

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
