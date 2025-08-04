#ifndef PLANT_H
#define PLANT_H

#include <QObject>
#include <QJsonObject>
#define id_t qint64

class Plant : public QObject
{
    Q_OBJECT

private:
    id_t plantId_;
    QString nickname_;
    int humidity_;
    double temperature_;
    id_t userId_;

public:
    explicit Plant(QObject *parent = nullptr);

    // setter : constructor
    Plant(id_t id, const QString& nickname, int humidity, double temperature, id_t userId)
        : plantId_(id), nickname_(nickname), humidity_(humidity), temperature_(temperature), userId_(userId) {}

    // getter
    id_t getPlantId() const;
    QString getNickname() const;
    int getHumidity() const;
    double getTemperature() const;
    id_t getUserId() const;

    // setter
    bool setPlantId(id_t id);
    bool setNickName(const QString& inputName);
    bool setHumidity(const int& humidity_data);
    bool setTemperature(const double& temperature_data);
    bool setUserId(const id_t& userId);

    // toJson, fromJson
    QJsonObject toJson() const;
    static Plant fromJson(const QJsonObject& plantObj);



signals:
};

#endif // PLANT_H
