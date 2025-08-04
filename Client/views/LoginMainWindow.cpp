#include "LoginMainWindow.h"
#include "qjsonobject.h"
#include "ui_login_main_window.h"


LoginMainWindow::LoginMainWindow(QWidget *parent,ClientSocket* client_socket)
    : QMainWindow(parent)
    , ui(new Ui::LoginMainWindow)
    , m_client(client_socket)
{
    m_register = new JoinMemberMainWindow(nullptr, m_client);
    ui->setupUi(this);
    connect(m_client, &ClientSocket::loginResponse,this, &LoginMainWindow::onLoginResponse);
    connect(m_client, &ClientSocket::logoutResponse,this, &LoginMainWindow::onLogoutResponse);
    this->show();
}

LoginMainWindow::~LoginMainWindow()
{
    m_client->logout();
    delete ui;
}

void LoginMainWindow::on_pushButton_login_clicked()
{
    QString strId = ui->lineEdit_id->text();
    QString password = ui->lineEdit_pw->text();

    m_client->login(strId, password);
}


void LoginMainWindow::on_pushButton_register_clicked()
{
    m_register->show();
}

void LoginMainWindow::onLoginResponse(bool success, const QJsonObject &userData)
{
    if (success) {
        QString name = userData.value("name").toString();
        QString sessionId = userData.value("sessionId").toString();
        int userId = userData.value("userId").toInt();
        m_isLoggedIn = true;

        qDebug() << QString("✅ 로그인 성공: %1 (ID: %2, 세션: %3)").arg(name).arg(userId).arg(sessionId);
        close();
    } else {
        qDebug() << "❌ 로그인 실패";
        m_isLoggedIn = false;
    }
}

void LoginMainWindow::onLogoutResponse(bool success)
{
    if (success) {
        qDebug() << "✅ 로그아웃 성공";
        m_isLoggedIn = false;
    } else {
        qDebug() << "❌ 로그아웃 실패";
    }
}


