#ifndef ORDERMANAGESERVICE_H
#define ORDERMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include "models/entity/order/Order.h"
#include "models/entity/order/OrderItem.h"
#include "models/repository/OrderJsonRepo.h"
#include "models/repository/OrderItemJsonRepo.h"
#include "utils/CustomErrorCodes.h"

enum class OrderStatus {
    Pending,
    Processing,
    Shipped,
    Delivered,
    Cancelled
};

class OrderManageService : public QObject
{
    Q_OBJECT

private:
    static OrderManageService* instance_;
    OrderJsonRepo* orderRepo_;
    OrderItemJsonRepo* orderItemRepo_;

    OrderManageService(QObject *parent = nullptr);
    OrderManageService(const OrderManageService&) = delete;
    OrderManageService& operator=(const OrderManageService&) = delete;

public:
    static OrderManageService* getInstance();
    static void destroyInstance();
    ~OrderManageService() = default;

    // 주문 관리 메서드들
    RaErrorCode createOrder(const Order& order);
    RaErrorCode updateOrder(const Order& order);
    RaErrorCode deleteOrder(id_t orderId);
    const Order* getOrderById(id_t orderId);
    QVector<Order> getOrdersByUserId(id_t userId);
    QVector<Order> getAllOrders();

    // 주문 아이템 관리
    RaErrorCode addOrderItem(const OrderItem& orderItem);
    RaErrorCode updateOrderItem(const OrderItem& orderItem);
    RaErrorCode removeOrderItem(id_t orderItemId);
    const OrderItem* getOrderItemById(id_t orderItemId);
    QVector<OrderItem> getOrderItemsByOrderId(id_t orderId);

    // 주문 처리
    RaErrorCode processOrder(id_t orderId);
    RaErrorCode cancelOrder(id_t orderId);
    RaErrorCode shipOrder(id_t orderId);
    RaErrorCode deliverOrder(id_t orderId);

    // 계산 메서드
    qreal calculateOrderTotal(id_t orderId);
    qreal calculateOrderItemTotal(id_t orderItemId);
    int getOrderItemCount(id_t orderId);

    // 데이터 관리
    bool loadOrders(const QString& orderFilePath, const QString& orderItemFilePath);
    bool saveOrders();
    int getOrderCount();

signals:
    void orderCreated(id_t orderId);
    void orderUpdated(id_t orderId);
    void orderDeleted(id_t orderId);
    void orderItemAdded(id_t orderId, id_t orderItemId);
    void orderItemRemoved(id_t orderId, id_t orderItemId);
    void orderStatusChanged(id_t orderId, OrderStatus newStatus);
};

#endif // ORDERMANAGESERVICE_H
