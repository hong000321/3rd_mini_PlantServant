#ifndef ORDERITEMJSONREPO_H
#define ORDERITEMJSONREPO_H
#include "JsonRepo.h"
#include "models/Entity/order/OrderItem.h"

class OrderItemJsonRepo : public JsonRepo<OrderItem> {
private:
    static OrderItemJsonRepo* instance_;  // 싱글톤 인스턴스
    OrderItemJsonRepo();
    OrderItemJsonRepo(const OrderItemJsonRepo&) = delete;
    OrderItemJsonRepo& operator=(const OrderItemJsonRepo&) = delete;
    OrderItemJsonRepo(OrderItemJsonRepo&&) = delete;
    OrderItemJsonRepo& operator=(OrderItemJsonRepo&&) = delete;

public:
    static OrderItemJsonRepo* getInstance();
    ~OrderItemJsonRepo() = default;
    static void destroyInstance();
};

#endif // ORDERITEMJSONREPO_H
