#ifndef LOGINMAINWINDOW_H
#define LOGINMAINWINDOW_H

#include <QMainWindow>
#include "src/controllers/ClientSocket.h"
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
    LoginMainWindow(QWidget *parent = nullptr, ClientSocket *client_socket = nullptr);
    ~LoginMainWindow();
    Ui::LoginMainWindow *ui;

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void onLoginResponse(bool success, const QJsonObject &userData);
    void onLogoutResponse(bool success);


private:
    bool m_isLoggedIn;
    JoinMemberMainWindow *m_register;
    ClientSocket *m_client;
};
#endif // LOGINMAINWINDOW_H
