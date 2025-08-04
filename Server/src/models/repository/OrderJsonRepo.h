#ifndef ORDERJSONREPO_H
#define ORDERJSONREPO_H
#include "JsonRepo.h"
#include "models/Entity/order/Order.h"

class OrderJsonRepo : public JsonRepo<Order> {
private:
    static OrderJsonRepo* instance_;  // 싱글톤 인스턴스
    OrderJsonRepo();
    OrderJsonRepo(const OrderJsonRepo&) = delete;
    OrderJsonRepo& operator=(const OrderJsonRepo&) = delete;
    OrderJsonRepo(OrderJsonRepo&&) = delete;
    OrderJsonRepo& operator=(OrderJsonRepo&&) = delete;

public:
    static OrderJsonRepo* getInstance();
    ~OrderJsonRepo() = default;
    static void destroyInstance();
};

#endif // ORDERJSONREPO_H
