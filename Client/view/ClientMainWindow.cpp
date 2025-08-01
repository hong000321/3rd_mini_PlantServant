#include "ClientMainWindow.h"
#include "ui_ClientMainWindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientMainWindow)
{
    ui->setupUi(this);
}

ClientMainWindow::~ClientMainWindow()
{
    delete ui;
}
