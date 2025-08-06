#ifndef SENSORDATAQUEUE_H
#define SENSORDATAQUEUE_H

#include <QObject>
#include <QMutex>
#include <queue>
#include "models/entity/SensorData.h"  // SensorData 구조체 사용

class SensorDataQueue : public QObject
{
    Q_OBJECT

private:
    static SensorDataQueue* instance_;
    std::queue<SensorData> dataQueue_;
    QMutex mutex_;

    SensorDataQueue(QObject *parent = nullptr);

public:
    static SensorDataQueue* getInstance();
    static void destroyInstance();

    // 핵심 기능만
    void enqueue(id_t plantId, double temperature, int humidity);
    bool tryDequeue(SensorData& data);
    bool isEmpty();
};

#endif // SENSORDATAQUEUE_H
