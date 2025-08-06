#ifndef SENSORPROCESSOR_H
#define SENSORPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "controllers/SensorDataQueue.h"
#include "models/repository/SensorDB.h"

// sensor 정보를 db에 저장하기 위한 소비자
class DBWorker : public QObject{
    Q_OBJECT
public slots:
    void doWork(const SensorData& data);
signals:
    void resultReady(const QString &result);
};

// plant service에 sensor정보 업데이트를 위한 소비자
class ServiceWorker : public QObject{
    Q_OBJECT
public slots:
    void doWork(const SensorData& data);
signals:
    void resultReady(const QString &result);
};

// 소비자 관리 프로세서
class SensorProcessor : public QObject
{
    Q_OBJECT

private:
    static SensorProcessor* instance_;
    DBWorker *dbWorker_;
    ServiceWorker *serviceWorker_;

    QThread* dbThread_;
    QThread* serviceThread_;

    SensorDataQueue* dataQueue_;
    SensorDB* sensorDB_;

    SensorProcessor(QObject *parent = nullptr);

public:
    static SensorProcessor* getInstance();
    static void destroyInstance();
    ~SensorProcessor();
    void sensorDataProcess();

    void start();
    void stop();

private slots:
    void handleResults(const QString &result);

signals:
    void processData(const SensorData& data);
};

#endif // SENSORPROCESSOR_H
