#ifndef PLANT_H
#define PLANT_H

#include "models/entity/Entity.h"

class Plant : public Entity
{
private:
    id_t plantId_;
    QString nickname_;
    int humidity_;
    double temperature_;
    id_t userId_;

public:
    Plant() = default;
    Plant(id_t id, const QString& nickname, int humidity, double temperature, id_t userId)
        : plantId_(id), nickname_(nickname), humidity_(humidity), temperature_(temperature), userId_(userId) {}

    // getter
    id_t getId() const override;
    QString getNickname() const;
    int getHumidity() const;
    double getTemperature() const;
    id_t getUserId() const;

    // setter
    void setId(id_t id) override;
    RaErrorCode setNickName(const QString& inputName);
    RaErrorCode setHumidity(const int& humidity_data);
    RaErrorCode setTemperature(const double& temperature_data);
    RaErrorCode setUserId(const id_t& userId);

    // json
    QJsonObject toJson() const override;
    RaErrorCode fromJson(const QJsonObject& inputJson) override;
};

#endif // PLANT_H
