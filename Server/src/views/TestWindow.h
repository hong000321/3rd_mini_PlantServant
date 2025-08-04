#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>
#include "ServerConfig.h"
#include "models/repository/UserJsonRepo.h"
#include "models/repository/ProductJsonRepo.h"
#include "models/repository/OrderJsonRepo.h"
#include "models/repository/OrderItemJsonRepo.h"
#include "models/repository/ChatJsonRepo.h"
#include "models/repository/ChatRoomJsonRepo.h"

namespace Ui {
class TestWindow;
}

class TestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);
    ~TestWindow();

private slots:
    void on_pushButton_userRepoTest_clicked();
    void on_lineEdit_userRepoPath_editingFinished();
    void on_pushButton_userRepoTest_pressed();

private:
    Ui::TestWindow *ui;
    ServerConfig *sc;
    UserJsonRepo *userRepo;
    ProductJsonRepo *productRepo;
    OrderJsonRepo *orderRepo;
    OrderItemJsonRepo *orderItemRepo;
    ChatJsonRepo *chatRepo;
    ChatRoomJsonRepo *chatRoomRepo;

};

#endif // TESTWINDOW_H
