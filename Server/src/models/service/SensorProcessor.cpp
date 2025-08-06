#include "SensorProcessor.h"
#include <QDebug>

SensorProcessor* SensorProcessor::instance_ = nullptr;

SensorProcessor::SensorProcessor(QObject *parent)
    : QObject(parent)
    , workerThread_(nullptr)
    , processTimer_(nullptr)
    , dataQueue_(SensorDataQueue::getInstance())
    , sensorDB_(SensorDB::getInstance())
{
}

SensorProcessor::~SensorProcessor()
{
    stop();
}

SensorProcessor* SensorProcessor::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new SensorProcessor();
    }
    return instance_;
}

void SensorProcessor::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void SensorProcessor::start()
{
    workerThread_ = new QThread();
    processTimer_ = new QTimer();

    processTimer_->moveToThread(workerThread_);
    this->moveToThread(workerThread_);

    connect(workerThread_, &QThread::started, processTimer_, [this]() {
        processTimer_->start(200); // 200ms마다 체크
    });

    connect(processTimer_, &QTimer::timeout, this, &SensorProcessor::processData);

    workerThread_->start();
    qDebug() << "[CONSUMER] SensorProcessor 시작";
}

void SensorProcessor::stop()
{
    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait();
        delete workerThread_;
        workerThread_ = nullptr;
        processTimer_ = nullptr;
    }
    qDebug() << "[CONSUMER] SensorProcessor 중지";
}

void SensorProcessor::processData()
{
    SensorData data;

    // 큐에서 데이터 읽어서 DB에 저장
    while (dataQueue_->tryDequeue(data)) {
        sensorDB_->addSensorData(data.plantId, data.temperature, data.humidity);
        qDebug() << "[CONSUMER] 처리 완료 - Plant:" << data.plantId;
    }
}
