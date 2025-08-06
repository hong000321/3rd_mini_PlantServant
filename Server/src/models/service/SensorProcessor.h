#ifndef SENSORPROCESSOR_H
#define SENSORPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "controllers/SensorDataQueue.h"
#include "models/repository/SensorDB.h"
class SensorProcessor : public QObject
{
    Q_OBJECT

private:
    static SensorProcessor* instance_;
    QThread* workerThread_;
    QTimer* processTimer_;
    SensorDataQueue* dataQueue_;
    SensorDB* sensorDB_;

    SensorProcessor(QObject *parent = nullptr);

public:
    static SensorProcessor* getInstance();
    static void destroyInstance();
    ~SensorProcessor();

    void start();
    void stop();

private slots:
    void processData();
};

#endif // SENSORPROCESSOR_H
