#ifndef PRODUCTMANAGESERVICE_H
#define PRODUCTMANAGESERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "models/entity/product/Product.h"
#include "models/repository/ProductJsonRepo.h"
#include "utils/CustomErrorCodes.h"

class ProductManageService : public QObject
{
    Q_OBJECT

private:
    static ProductManageService* instance;
    ProductJsonRepo* productRepo;

    ProductManageService(QObject *parent = nullptr);
    ProductManageService(const ProductManageService&) = delete;
    ProductManageService& operator=(const ProductManageService&) = delete;

public:
    static ProductManageService* getInstance();
    static void destroyInstance();
    ~ProductManageService() = default;

    // 제품 관리 메서드들
    RaErrorCode createProduct(const Product& product);
    RaErrorCode updateProduct(const Product& product);
    RaErrorCode deleteProduct(id_t productId);
    const Product* getProductById(id_t productId);
    QVector<Product> getProductsByCategory(const QString& category);
    QVector<Product> getAllProducts();

    // 재고 관리
    RaErrorCode increaseStock(id_t productId, quint32 quantity);
    RaErrorCode decreaseStock(id_t productId, quint32 quantity);
    qint32 getStock(id_t productId);
    bool isInStock(id_t productId, quint32 requiredQuantity = 1);

    // 가격 관리
    RaErrorCode updatePrice(id_t productId, qreal newPrice);
    qreal getPrice(id_t productId);

    // 검색 기능
    QVector<Product> searchProductsByName(const QString& name);
    QVector<Product> getProductsInPriceRange(qreal minPrice, qreal maxPrice);

    // 데이터 관리
    bool loadProducts(const QString& filePath);
    bool saveProducts();
    int getProductCount();

signals:
    void productCreated(id_t productId);
    void productUpdated(id_t productId);
    void productDeleted(id_t productId);
    void stockChanged(id_t productId, qint32 newStock);
    void priceChanged(id_t productId, qreal newPrice);
};

#endif // PRODUCTMANAGESERVICE_H
