#ifndef HTTPRESTSERVER_H
#define HTTPRESTSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QHttpServerRequest>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
#include "SensorDataQueue.h"

class HttpRestServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpRestServer(QObject *parent = nullptr);

    void start(quint16 port = 8080); //http 포트 바인딩

private slots:
    void setupRoutes(); //route 설정할거임
    QHttpServerResponse postSensorData(const QHttpServerRequest &request);

signals:

private:
    QHttpServer server_;
    // QHash<int, QJsonValue> users_; // 이건 왜 넣었지요
    // int nextId_ = 1; // 이것도 왜 넣었는지 몰?루겠어요
    QTcpServer *tcpServer_ = nullptr;
};

#endif // HTTPRESTSERVER_H
