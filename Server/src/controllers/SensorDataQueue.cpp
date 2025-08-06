#include "SensorDataQueue.h"
#include <QDateTime>
#include <QDebug>

SensorDataQueue* SensorDataQueue::instance_ = nullptr;

SensorDataQueue::SensorDataQueue(QObject *parent) : QObject(parent)
{
}

SensorDataQueue* SensorDataQueue::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new SensorDataQueue();
    }
    return instance_;
}

void SensorDataQueue::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void SensorDataQueue::enqueue(id_t plantId, double temperature, int humidity)
{
    QMutexLocker locker(&mutex_);

    SensorData data;
    data.plantId = plantId;
    data.temperature = temperature;
    data.humidity = humidity;
    data.timestamp = QDateTime::currentDateTime();

    dataQueue_.push(data);
    qDebug() << "[PRODUCER] 데이터 추가 - Plant:" << plantId << "큐 크기:" << dataQueue_.size();
}

bool SensorDataQueue::tryDequeue(SensorData& data)
{
    QMutexLocker locker(&mutex_);

    if (dataQueue_.empty()) {
        return false;
    }

    data = dataQueue_.front();
    dataQueue_.pop();
    qDebug() << "[CONSUMER] 데이터 읽음 - Plant:" << data.plantId << "남은 큐 크기:" << dataQueue_.size();

    return true;
}

bool SensorDataQueue::isEmpty()
{
    QMutexLocker locker(&mutex_);
    return dataQueue_.empty();
}
