#ifndef LOGINMAINWINDOW_H
#define LOGINMAINWINDOW_H

#include <QMainWindow>
#include "models/service/UserService.h"
#include "JoinMemberMainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginMainWindow;
}
QT_END_NAMESPACE

class LoginMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginMainWindow(QWidget *parent = nullptr, UserService* userService = nullptr);
    ~LoginMainWindow();
    Ui::LoginMainWindow *ui;

signals:
    void loginSuccess(const QJsonObject& userInfo);  // 외부로 알려줄 용도

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void onLoginSuccess(const QJsonObject &userData);
    void onLoginFailed(const QString& reason);

private:
    bool isLoggedIn_;
    JoinMemberMainWindow* register_;
    UserService* userService_;
};

#endif // LOGINMAINWINDOW_H
