#include "ProductManageService.h"
#include <QDebug>

ProductManageService* ProductManageService::instance = nullptr;

ProductManageService::ProductManageService(QObject *parent)
    : QObject(parent)
{
    productRepo = ProductJsonRepo::getInstance();
}

ProductManageService* ProductManageService::getInstance()
{
    if (instance == nullptr) {
        instance = new ProductManageService();
    }
    return instance;
}

void ProductManageService::destroyInstance()
{
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

RaErrorCode ProductManageService::createProduct(const Product& product)
{
    id_t productId = productRepo->insert(product);
    if (productId >= 0) {
        emit productCreated(productId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ProductManageService::updateProduct(const Product& product)
{
    const Product* existingProduct = productRepo->getObjPtrById(product.getId());
    if (existingProduct == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    if (productRepo->update(product)) {
        emit productUpdated(product.getId());
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

RaErrorCode ProductManageService::deleteProduct(id_t productId)
{
    if (productRepo->removeById(productId)) {
        emit productDeleted(productId);
        return Ra_Success;
    }

    return Ra_Domain_Unkown_Error;
}

const Product* ProductManageService::getProductById(id_t productId)
{
    return productRepo->getObjPtrById(productId);
}

QVector<Product> ProductManageService::getProductsByCategory(const QString& category)
{
    QVector<Product> result;
    QVector<Product> allProducts = productRepo->getAllObjects();

    for (const Product& product : allProducts) {
        if (product.getCategory() == category) {
            result.append(product);
        }
    }

    return result;
}

QVector<Product> ProductManageService::getAllProducts()
{
    return productRepo->getAllObjects();
}

RaErrorCode ProductManageService::increaseStock(id_t productId, quint32 quantity)
{
    const Product* product = productRepo->getObjPtrById(productId);
    if (product == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Product updatedProduct = *product;
    RaErrorCode result = updatedProduct.increaseStock(quantity);
    if (result == Ra_Success) {
        productRepo->update(updatedProduct);
        emit stockChanged(productId, updatedProduct.getStock());
    }

    return result;
}

RaErrorCode ProductManageService::decreaseStock(id_t productId, quint32 quantity)
{
    const Product* product = productRepo->getObjPtrById(productId);
    if (product == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Product updatedProduct = *product;
    RaErrorCode result = updatedProduct.decreaseStock(quantity);
    if (result == Ra_Success) {
        productRepo->update(updatedProduct);
        emit stockChanged(productId, updatedProduct.getStock());
    }

    return result;
}

qint32 ProductManageService::getStock(id_t productId)
{
    const Product* product = productRepo->getObjPtrById(productId);
    return product ? product->getStock() : -1;
}

bool ProductManageService::isInStock(id_t productId, quint32 requiredQuantity)
{
    const Product* product = productRepo->getObjPtrById(productId);
    return product && product->getStock() >= static_cast<qint32>(requiredQuantity);
}

RaErrorCode ProductManageService::updatePrice(id_t productId, qreal newPrice)
{
    const Product* product = productRepo->getObjPtrById(productId);
    if (product == nullptr) {
        return Ra_Domain_Unkown_Error;
    }

    Product updatedProduct = *product;
    RaErrorCode result = updatedProduct.setPrice(newPrice);
    if (result == Ra_Success) {
        productRepo->update(updatedProduct);
        emit priceChanged(productId, newPrice);
    }

    return result;
}

qreal ProductManageService::getPrice(id_t productId)
{
    const Product* product = productRepo->getObjPtrById(productId);
    return product ? product->getPrice() : -1.0;
}

QVector<Product> ProductManageService::searchProductsByName(const QString& name)
{
    QVector<Product> result;
    QVector<Product> allProducts = productRepo->getAllObjects();

    for (const Product& product : allProducts) {
        if (product.getName().contains(name, Qt::CaseInsensitive)) {
            result.append(product);
        }
    }

    return result;
}

QVector<Product> ProductManageService::getProductsInPriceRange(qreal minPrice, qreal maxPrice)
{
    QVector<Product> result;
    QVector<Product> allProducts = productRepo->getAllObjects();

    for (const Product& product : allProducts) {
        if (product.getPrice() >= minPrice && product.getPrice() <= maxPrice) {
            result.append(product);
        }
    }

    return result;
}

bool ProductManageService::loadProducts(const QString& filePath)
{
    return productRepo->loadDataFromFile(filePath);
}

bool ProductManageService::saveProducts()
{
    return productRepo->saveToFile();
}

int ProductManageService::getProductCount()
{
    return productRepo->getSize();
}
