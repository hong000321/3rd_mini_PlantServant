#ifndef JSONREPO_H
#define JSONREPO_H
#include "models/entity/Entity.h"
#include "IRepository.h"
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

template <typename T>
class JsonRepo : public IRepository<T>
{
protected:
    using IRepository<T>::vectorData_;
    using IRepository<T>::filepath_;

public:
    JsonRepo() : IRepository<T>(){}
    ~JsonRepo() = default;  // 부모 클래스에서 메모리 정리
    bool saveToFile() override {
        QJsonArray array;

        for(T* object : vectorData_){
            QJsonObject jsonobj = object->toJson();  // Entity* 캐스팅 제거
            array.append(jsonobj);
        }
        QJsonDocument doc(array);

        QFile file(filepath_);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        file.write(doc.toJson());
        file.close();
        return true;
    }

    bool loadDataFromFile(const QString& path) override {
        filepath_ = path;
        qDebug() << "start load data1!!! " << filepath_;

        // 기존 데이터 정리
        for (T* obj : vectorData_) {
            delete obj;
        }
        vectorData_.clear();

        QFile file(filepath_);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file:" << filepath_;
            return false;
        }

        qDebug() << "file read success !!! " << file.size();
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (doc.isNull()) {
            qDebug() << "Failed to parse JSON document";
            return false;
        }

        QJsonArray array = doc.array();
        qDebug() << "start load data2!!! " << array.size();

        if(array.size() == 0){
            qDebug() << "Empty JSON array";
            return true;  // 빈 배열은 성공으로 처리
        }

        for (const auto& value : std::as_const(array)) {
            qDebug() << "start save object!!!";

            if (!value.isObject()) {
                qDebug() << "Invalid JSON value - not an object";
                continue;
            }

            T* object = new T();  // 동적 할당
            RaErrorCode result = object->fromJson(value.toObject());

            if (result == Ra_Success) {
                vectorData_.append(object);
                qDebug() << "Object loaded successfully, ID:" << object->getId();
            } else {
                qDebug() << "Failed to parse JSON object, error code:" << result;
                delete object;  // 실패 시 메모리 해제
            }
        }
        return true;
    }

    bool appendToFile() override {
        return saveToFile();
    }

};

#endif // JSONREPO_H
