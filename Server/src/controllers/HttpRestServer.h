#ifndef HTTPRESTSERVER_H
#define HTTPRESTSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QHttpServerRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTimer>
class HttpRestServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpRestServer(QObject *parent = nullptr);

    // void start(quint16 port = 55555);

private slots:
    // void setupRoutes();
    // QHttpServerResponse postSensorData();

signals:

private:
    QHttpServer server_;
    QHash<int, QJsonValue> users_;
    int nextId_ = 1;
};

#endif // HTTPRESTSERVER_H
