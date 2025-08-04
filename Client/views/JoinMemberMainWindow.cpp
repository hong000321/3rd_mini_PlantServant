#include "JoinMemberMainWindow.h"
#include "ui_JoinMemberMainWindow.h"
#include <QDebug>

JoinMemberMainWindow::JoinMemberMainWindow(QWidget *parent, UserService* userService)
    : QMainWindow(parent)
    , ui(new Ui::JoinMemberMainWindow)
    , userService_(userService)
{
    ui->setupUi(this);

    connect(userService_, &UserService::registrationSuccess,
            this, &JoinMemberMainWindow::onRegistrationSuccess);
    connect(userService_, &UserService::registrationFailed,
            this, &JoinMemberMainWindow::onRegistrationFailed);
}

JoinMemberMainWindow::~JoinMemberMainWindow()
{
    delete ui;
}

void JoinMemberMainWindow::on_pushButton_register_clicked()
{
    QString strId = ui->lineEdit_id->text();
    QString password = ui->lineEdit_pw->text();
    QString name = ui->lineEdit_name->text();
    QString email = ui->lineEdit_email->text();
    QString address = ui->lineEdit_address->text();

    userService_->registerUser(strId, password, name, email, address);
}

void JoinMemberMainWindow::onRegistrationSuccess(const QJsonObject &userData)
{
    QString name = userData.value("name").toString();
    qDebug() << QString("✅ 회원가입 성공: %1").arg(name);
    ui->lineEdit_id->clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_pw->clear();
    ui->lineEdit_email->clear();
    ui->lineEdit_address->clear();
    close();
}

void JoinMemberMainWindow::onRegistrationFailed(const QString& reason)
{
    qDebug() << "❌ 회원가입 실패:" << reason;
}
