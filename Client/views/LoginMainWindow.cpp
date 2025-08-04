#include "LoginMainWindow.h"
#include "ui_LoginMainWindow.h"

#include <QJsonObject>
#include <QDebug>

LoginMainWindow::LoginMainWindow(QWidget *parent, UserService* userService)
    : QMainWindow(parent)
    , ui(new Ui::LoginMainWindow)
    , isLoggedIn_(false)
    , userService_(userService)

{
    ui->setupUi(this);

    // JoinMember 창도 같은 userService 사용
    register_ = new JoinMemberMainWindow(nullptr, userService_);

    connect(userService_, &UserService::loginSuccess, this, &LoginMainWindow::onLoginSuccess);
    connect(userService_, &UserService::loginFailed, this, &LoginMainWindow::onLoginFailed);

    this->show();
}

LoginMainWindow::~LoginMainWindow()
{
    userService_->logout();
    delete ui;
}

void LoginMainWindow::on_pushButton_login_clicked()
{

    QString strId = ui->lineEdit_id->text();
    QString password = ui->lineEdit_pw->text();
    userService_->login(strId, password);
}

void LoginMainWindow::on_pushButton_register_clicked()
{
    register_->show();
}

void LoginMainWindow::onLoginSuccess(const QJsonObject &userData)
{
    QString name = userData.value("name").toString();
    QString sessionId = userData.value("sessionId").toString();
    int userId = userData.value("userId").toInt();
    isLoggedIn_ = true;

    qDebug() << QString("✅ 로그인 성공: %1 (ID: %2, 세션: %3)").arg(name).arg(userId).arg(sessionId);

    emit loginSuccess(userData);  // 외부에서 감지 가능
    close();
}

void LoginMainWindow::onLoginFailed(const QString& reason)
{
    qDebug() << "❌ 로그인 실패:" << reason;
    isLoggedIn_ = false;
}
