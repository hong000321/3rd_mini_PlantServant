#ifndef PRODUCTJSONREPO_H
#define PRODUCTJSONREPO_H
#include "JsonRepo.h"
#include "models/Entity/product/Product.h"

class ProductJsonRepo : public JsonRepo<Product> {
private:
    static ProductJsonRepo* instance_;  // 싱글톤 인스턴스
    ProductJsonRepo();
    ProductJsonRepo(const ProductJsonRepo&) = delete;
    ProductJsonRepo& operator=(const ProductJsonRepo&) = delete;
    ProductJsonRepo(ProductJsonRepo&&) = delete;
    ProductJsonRepo& operator=(ProductJsonRepo&&) = delete;

public:
    static ProductJsonRepo* getInstance();
    ~ProductJsonRepo() = default;
    static void destroyInstance();
};

#endif // PRODUCTJSONREPO_H
