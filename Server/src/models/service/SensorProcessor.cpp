#include "SensorProcessor.h"
#include <QDebug>

void DBWorker::doWork(const SensorData& data){

}

void ServiceWorker::doWork(const SensorData& data){

}


SensorProcessor* SensorProcessor::instance_ = nullptr;

SensorProcessor::SensorProcessor(QObject *parent)
    : QObject(parent)
    , dbThread_(nullptr)
    , serviceThread_(nullptr)
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

void SensorProcessor::sensorDataProcess(){

    SensorData data;
    // 큐에서 데이터 읽어서 DB에 저장
    while (dataQueue_->tryDequeue(data)) {
        sensorDB_->addSensorData(data.plantId, data.temperature, data.humidity);
        qDebug() << "[CONSUMER] 처리 완료 - Plant:" << data.plantId;
    }
    emit processData(data);
}

void SensorProcessor::start()
{
    dbWorker_ = new DBWorker;
    serviceWorker_ = new ServiceWorker;

    dbThread_ = new QThread();
    serviceThread_ = new QThread();

    dbWorker_->moveToThread(dbThread_);
    serviceWorker_->moveToThread(serviceThread_);

    connect(dbThread_, &QThread::finished, dbWorker_, &QObject::deleteLater);
    connect(this, &SensorProcessor::processData, dbWorker_, &DBWorker::doWork);
    connect(dbWorker_, &DBWorker::resultReady, this, &SensorProcessor::handleResults);

    connect(serviceThread_, &QThread::finished, serviceWorker_, &QObject::deleteLater);
    connect(this, &SensorProcessor::processData, serviceWorker_, &ServiceWorker::doWork);
    connect(serviceWorker_, &ServiceWorker::resultReady, this, &SensorProcessor::handleResults);


    dbThread_->start();
    serviceThread_->start();
    qDebug() << "[CONSUMER] SensorProcessor 시작";
}

void SensorProcessor::stop()
{
    if (dbThread_) {
        dbThread_->quit();
        dbThread_->wait();
        delete dbThread_;
        dbThread_ = nullptr;
    }

    if (serviceThread_) {
        serviceThread_->quit();
        serviceThread_->wait();
        delete serviceThread_;
        serviceThread_ = nullptr;
    }
    qDebug() << "[CONSUMER] SensorProcessor 중지";
}


void SensorProcessor::handleResults(const QString &result){
    qDebug() << "SensorProcessor result : "<< result;
}
