#ifndef IREPOSITORY_H
#define IREPOSITORY_H

#include <QVector>
#include <QString>
#include "models/entity/Entity.h"
#include "utils/CustomErrorCodes.h"

template <typename T>
struct CustomComparator {
    bool operator()(T* a, T* b) const {
        return a->getId() > b->getId();  // 내림차순
    }
};

template <typename T>
class IRepository {
protected:
    QVector<T*> vectorData; // T*로 변경 - 포인터 사용
    QString filepath;

public:
    IRepository() = default;

    virtual ~IRepository() {
        // 메모리 해제
        for (T* obj : vectorData) {
            delete obj;
        }
        vectorData.clear();
    }

    /**
     * @brief 파일에서 데이터를 로드하는 순수 가상 함수
     * @param path : 파일 경로
     * @return bool 성공 여부
     */
    virtual bool loadDataFromFile(const QString& path) = 0;

    /**
     * @brief 전체 데이터를 파일에 저장하는 순수 가상 함수
     * @return bool 성공 여부
     */
    virtual bool saveToFile() = 0;

    /**
     * @brief 새로운 데이터를 파일에 추가하는 순수 가상 함수
     * @return bool 성공 여부
     */
    virtual bool appendToFile() = 0;

    /**
     * @brief 새로운 아이템을 삽입하는 메서드
     * @param object : 삽입할 아이템 (복사본으로 전달)
     * @return bool 성공 여부
     */
    id_t insert(const T& object) {
        T* newObject = new T(object);  // 복사 생성자 사용
        sortVector();
        if(newObject->getId() < 0){
            newObject->setId(getLastId()+1);
        }
        if(getObjPtrById(newObject->getId()) != nullptr){
            newObject->setId(getLastId()+1);
        }
        vectorData.push_back(newObject);
        appendToFile();
        return newObject->getId();
    }

    /**
     * @brief 기존 아이템을 업데이트하는 메서드
     * @param inputObject : 업데이트할 아이템
     * @return bool 성공 여부
     */
    bool update(const T& inputObject) {
        for (T* object : vectorData) {
            if (object->getId() == inputObject.getId()) {
                *object = inputObject;  // 값 복사
                saveToFile();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief ID로 아이템을 삭제하는 메서드
     * @param id : 삭제할 아이템의 ID
     * @return bool 성공 여부
     */
    bool removeById(int id) {
        for(auto it = vectorData.begin(); it != vectorData.end(); ++it){
            if((*it)->getId() == id){
                delete *it;  // 메모리 해제
                vectorData.erase(it);
                saveToFile();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief ID로 아이템을 검색하는 메서드
     * @param id : 검색할 아이템의 ID
     * @return T* 아이템 포인터 (없으면 nullptr)
     */
    T* getObjPtrById(int id) {
        for(int i=0; i<vectorData.size() ; i++){
            T* object = vectorData[i];
            if (object->getId() == id) {
                return object;
            }
        }
        return nullptr;
    }

    /**
     * @brief 모든 아이템을 반환하는 메서드
     * @return QVector<T> 모든 아이템의 복사본
     */
    QVector<T> getAllObjects() {
        QVector<T> result;
        for (T* obj : vectorData) {
            result.append(*obj);
        }
        return result;
    }

    /**
     * @brief 저장된 아이템의 개수를 반환하는 메서드
     * @return int 아이템 개수
     */
    int getSize(){
        return vectorData.size();
    }

    /**
     * @brief 마지막 아이템의 ID를 반환하는 메서드
     * @return int 마지막 ID
     */
    id_t getLastId(){
        if(vectorData.size()==0)
            return 0;  // -1 대신 0 반환
        return vectorData.back()->getId();
    }

    id_t getFirstId(){
        if(vectorData.size()==0)
            return -1;
        return vectorData.front()->getId();
    }

    id_t getNextId(id_t id){
        int index = getIndexById(id);

        if(vectorData.size()==0)
            return -1;
        if(vectorData.size()<=(index+1))
            return -2;
        return vectorData[index+1]->getId();
    }

    void sortVector(){
        std::sort(vectorData.begin(), vectorData.end(),
                  [](const T* a, const T* b) {
                      return a->getId() < b->getId();
                  });
    }

private:
    int getIndexById(int id) {
        for(int i=0; i<vectorData.size() ; i++){
            T* object = vectorData[i];
            if (object->getId() == id) {
                return i;
            }
        }
        return -1;
    }
};

#endif // IREPOSITORY_H
