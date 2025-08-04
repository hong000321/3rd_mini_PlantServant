#include "TestWindow.h"
#include "ui_test_window.h"

TestWindow::TestWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestWindow)
{
    ui->setupUi(this);
    sc = ServerConfig::getInstance();

    userRepo = UserJsonRepo::getInstance();
    productRepo = ProductJsonRepo::getInstance();
    orderRepo = OrderJsonRepo::getInstance();
    orderItemRepo = OrderItemJsonRepo::getInstance();
    chatRepo = ChatJsonRepo::getInstance();
    chatRoomRepo = ChatRoomJsonRepo::getInstance();

    ui->lineEdit_userRepoPath->setText(sc->userFilePath);


}

TestWindow::~TestWindow()
{
    delete ui;
}


void TestWindow::on_pushButton_userRepoTest_clicked()
{
    qDebug() << sc->userFilePath;
    bool ret = userRepo->loadDataFromFile(sc->userFilePath);
    if(ret == false){
        qDebug() << "fail to load json file";
        return;
    }
    id_t id = userRepo->getLastId();
    qDebug() << "last id = " << id;
    id = userRepo->getFirstId();
    qDebug() << "first id = " << id;
    userRepo->sortVector();
    while(1){
        qDebug() << userRepo->getObjPtrById(id)->getId();
        qDebug() << userRepo->getObjPtrById(id)->getName();
        qDebug() << userRepo->getObjPtrById(id)->getEmail();
        qDebug() << Qt::endl;
        id = userRepo->getNextId(id);
        if(id < 0){
            break;
        }
    }
    id = userRepo->insert(User("helloUser", "hellopass","hello", "hello@gmail.com", "부산산다 이자식아",PermissionLevel::USER_LEVEL,false));

    qDebug() << userRepo->getObjPtrById(id)->getId();
    qDebug() << userRepo->getObjPtrById(id)->getName();
    qDebug() << userRepo->getObjPtrById(id)->getEmail();
    qDebug() << Qt::endl;
}


void TestWindow::on_lineEdit_userRepoPath_editingFinished()
{
    sc->userFilePath = ui->lineEdit_userRepoPath->text();
}

void TestWindow::on_pushButton_userRepoTest_pressed(){

}
