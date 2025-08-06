#ifndef PLANTSERVICE_H
#define PLANTSERVICE_H

#include <QObject>
#include <QJsonObject>
#include "controllers/ClientSocket.h"
#include "models/entity/Plant.h"

class PlantService : public QObject
{
    Q_OBJECT

public:
    explicit PlantService(ClientSocket* socket, QObject* parent = nullptr);

    void requestPlantInfo(id_t userId);

signals:
    void plantInfoReady(const Plant& plant);

private slots:
    void onPlantReceived(const QJsonObject& plant);

private:
    ClientSocket* socket_;
};

#endif // PLANTSERVICE_H
