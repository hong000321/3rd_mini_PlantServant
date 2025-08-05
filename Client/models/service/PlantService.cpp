#include "PlantService.h"
#include <QJsonDocument>
#include <QDebug>

PlantService::PlantService(ClientSocket* socket, QObject* parent)
    : QObject(parent), socket_(socket)
{
    connect(socket_, &ClientSocket::plantReceived,
            this, &PlantService::onPlantReceived);
}

void PlantService::requestPlantInfo(id_t userId)
{
    QJsonObject params;
    params["userId"] = static_cast<int>(userId);

    QJsonObject message = socket_->createCommandMessage("get", "plant", params);
    socket_->sendMessage(message);

    qDebug() << "📤 식물 정보 요청 전송:" << QJsonDocument(message).toJson();
}

void PlantService::onPlantReceived(const QJsonObject& plantObj)
{
    Plant plant;
    plant.fromJson(plantObj);
    emit plantInfoReady(plant);
}
