#ifndef ENTITY_H
#define ENTITY_H

#include <QJsonObject>
#include "utils/CustomErrorCodes.h"

#define id_t qint64

class Entity
{
public:
    Entity() = default;
    virtual QJsonObject toJson() const = 0;
    virtual RaErrorCode fromJson(const QJsonObject& inputJson) = 0;

    virtual id_t getId() const = 0;
    virtual void setId(id_t id) = 0;
};

#endif // ENTITY_H
