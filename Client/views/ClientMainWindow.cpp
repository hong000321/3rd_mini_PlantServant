#include "ClientMainWindow.h"
#include "ui_ClientMainWindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);

}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}


void ClientMainWindow::on_button_myplant_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_plant);
}


void ClientMainWindow::on_button_writepost_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_write_post);
}

// 뒤로 가기 버튼은 다 page_gallery로 이동한다
void ClientMainWindow::on_button_back_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}

void ClientMainWindow::on_button_back2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}

void ClientMainWindow::on_button_back3_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_gallery);
}



