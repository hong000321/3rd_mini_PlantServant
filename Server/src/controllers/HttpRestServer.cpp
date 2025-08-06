#include "HttpRestServer.h"
#include "models/repository/SensorDB.h"
#include "models/service/UserManageService.h"

HttpRestServer::HttpRestServer(QObject *parent)
    : QObject(parent)
{
}

void HttpRestServer::start(quint16 port)
{
    setupRoutes();

    tcpServer_ = new QTcpServer(this);
    if (!tcpServer_->listen(QHostAddress::Any, port) || !server_.bind(tcpServer_)) {
        qCritical() << "[HTTP REST SERVER] 포트 바인딩 실패:" << port;
        return;
    }

    qDebug() << "[HTTP REST SERVER] Listening on port" << tcpServer_->serverPort();
}

void HttpRestServer::setupRoutes()
{

    // 센서 데이터 POST 수신
    server_.route("/rasp/sensor", QHttpServerRequest::Method::Post,
                  [this](const QHttpServerRequest &req) {
                      return postSensorData(req);
                  });
}

QHttpServerResponse HttpRestServer::postSensorData(const QHttpServerRequest &request){
    const QByteArray body = request.body();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(body, &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "[HTTP REST SERVER] JSON 파싱 오류:" << error.errorString();
        return QHttpServerResponse("Invalid JSON", QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonObject obj = doc.object();
    double temp = obj["temperature"].toDouble();
    int humi = obj["humidity"].toInt();

    // DB 저장 ,파일 기록 로직 추가 가능
    // SensorDB *sensorDB = SensorDB::getInstance();
    UserManageService *userService = UserManageService::getInstance();
    SensorDataQueue *sensorQueue = SensorDataQueue::getInstance();

    QVector<User> users = userService->getAllUsers();

    for(auto &user : users){
        if (sensorQueue->enqueue(user.getPlantId(), temp, humi)) {
            qDebug() << "[PRODUCER] 센서 데이터 큐에 추가:" << "Plant:" << user.getPlantId()
                     << "Temp:" << temp << "Humidity:" << humi;

            // 즉시 응답 (DB 저장을 기다리지 않음)
            return QHttpServerResponse("OK", QHttpServerResponse::StatusCode::Ok);
        } else {
            qWarning() << "[PRODUCER] 큐가 가득참 - 데이터 추가 실패";
            return QHttpServerResponse("Queue Full", QHttpServerResponse::StatusCode::ServiceUnavailable);
        }
        // sensorDB->addSensorData(user.getPlantId(),temp,humi);

    }



    qDebug() << "라즈베리파이로부터 수신:";
    qDebug() << "  온도:" << temp << "  습도:" << humi;

    return QHttpServerResponse("OK", QHttpServerResponse::StatusCode::Ok);
}
