#include "OrderManageService.h"
#include <QDebug>

OrderManageService* OrderManageService::instance_ = nullptr;

OrderManageService::OrderManageService(QObject *parent)
    : QObject(parent)
{
    orderRepo_ = OrderJsonRepo::getInstance();
    orderItemRepo_ = OrderItemJsonRepo::getInstance();
}

OrderManageService* OrderManageService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new OrderManageService();
    }
    return instance_;
}

void OrderManageService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode OrderManageService::createOrder(const Order& order)
{
    id_t orderId = orderRepo_->insert(order);
    if (orderId >= 0) {
        emit orderCreated(orderId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode OrderManageService::updateOrder(const Order& order)
{
    const Order* existingOrder = orderRepo_->getObjPtrById(order.getId());
    if (existingOrder == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (orderRepo_->update(order)) {
        emit orderUpdated(order.getId());
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode OrderManageService::deleteOrder(id_t orderId)
{
    if (orderRepo_->removeById(orderId)) {
        emit orderDeleted(orderId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const Order* OrderManageService::getOrderById(id_t orderId)
{
    return orderRepo_->getObjPtrById(orderId);
}

QVector<Order> OrderManageService::getOrdersByUserId(id_t userId)
{
    QVector<Order> result;
    QVector<Order> allOrders = orderRepo_->getAllObjects();

    for (const Order& order : allOrders) {
        if (order.getUserId() == userId) {
            result.append(order);
        }
    }

    return result;
}

QVector<Order> OrderManageService::getAllOrders()
{
    return orderRepo_->getAllObjects();
}

RaErrorCode OrderManageService::addOrderItem(const OrderItem& orderItem)
{
    id_t itemId = orderItemRepo_->insert(orderItem);
    if (itemId >= 0) {
        emit orderItemAdded(0, itemId); // orderId는 별도로 관리 필요
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode OrderManageService::updateOrderItem(const OrderItem& orderItem)
{
    const OrderItem* existingItem = orderItemRepo_->getObjPtrById(orderItem.getId());
    if (existingItem == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (orderItemRepo_->update(orderItem)) {
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode OrderManageService::removeOrderItem(id_t orderItemId)
{
    if (orderItemRepo_->removeById(orderItemId)) {
        emit orderItemRemoved(0, orderItemId); // orderId는 별도로 관리 필요
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const OrderItem* OrderManageService::getOrderItemById(id_t orderItemId)
{
    return orderItemRepo_->getObjPtrById(orderItemId);
}

QVector<OrderItem> OrderManageService::getOrderItemsByOrderId(id_t orderId)
{
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return QVector<OrderItem>();
    }

    QVector<OrderItem> result;
    QVector<id_t> itemIds = order->getItemIds();

    for (id_t itemId : itemIds) {
        const OrderItem* item = orderItemRepo_->getObjPtrById(itemId);
        if (item != nullptr) {
            result.append(*item);
        }
    }

    return result;
}

RaErrorCode OrderManageService::processOrder(id_t orderId)
{
    // 주문 처리 로직 구현
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    // 여기에 주문 처리 로직 추가
    emit orderStatusChanged(orderId, OrderStatus::Processing);
    return Ra_Success;
}

RaErrorCode OrderManageService::cancelOrder(id_t orderId)
{
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    emit orderStatusChanged(orderId, OrderStatus::Cancelled);
    return Ra_Success;
}

RaErrorCode OrderManageService::shipOrder(id_t orderId)
{
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    emit orderStatusChanged(orderId, OrderStatus::Shipped);
    return Ra_Success;
}

RaErrorCode OrderManageService::deliverOrder(id_t orderId)
{
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    emit orderStatusChanged(orderId, OrderStatus::Delivered);
    return Ra_Success;
}

qreal OrderManageService::calculateOrderTotal(id_t orderId)
{
    const Order* order = orderRepo_->getObjPtrById(orderId);
    if (order == nullptr) {
        return 0.0;
    }

    qreal total = 0.0;
    QVector<id_t> itemIds = order->getItemIds();

    for (id_t itemId : itemIds) {
        const OrderItem* item = orderItemRepo_->getObjPtrById(itemId);
        if (item != nullptr) {
            total += item->getTotlaPrice();
        }
    }

    return total;
}

qreal OrderManageService::calculateOrderItemTotal(id_t orderItemId)
{
    const OrderItem* item = orderItemRepo_->getObjPtrById(orderItemId);
    return item ? item->getTotlaPrice() : 0.0;
}

int OrderManageService::getOrderItemCount(id_t orderId)
{
    return getOrderItemsByOrderId(orderId).size();
}

bool OrderManageService::loadOrders(const QString& orderFilePath, const QString& orderItemFilePath)
{
    bool orderResult = orderRepo_->loadDataFromFile(orderFilePath);
    bool itemResult = orderItemRepo_->loadDataFromFile(orderItemFilePath);
    return orderResult && itemResult;
}

bool OrderManageService::saveOrders()
{
    bool orderResult = orderRepo_->saveToFile();
    bool itemResult = orderItemRepo_->saveToFile();
    return orderResult && itemResult;
}

int OrderManageService::getOrderCount()
{
    return orderRepo_->getSize();
}
