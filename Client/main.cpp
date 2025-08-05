#include <QObject>
#include <QApplication>

#include "ClientMainWindow.h"
#include "JoinMemberMainWindow.h"
#include "LoginMainWindow.h"
#include "ClientMainWindow.h"
#include "ClientSocket.h"
#include "models/service/UserService.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientSocket* socket = new ClientSocket(); //서버 연결 아직 X
    socket->connectToServer("192.168.2.180",54321); //서버 연결

    UserService* userService = new UserService(socket); // 로그인, 회원가입, 로그아웃 담당

    ClientMainWindow* clientMainWindow = new ClientMainWindow(socket, userService);
    LoginMainWindow* loginWindow = new LoginMainWindow(nullptr, userService);

    //connect
    QObject::connect(loginWindow, &LoginMainWindow::loginSuccess,
                     [&] (const QJsonObject& userData) {
                         clientMainWindow->show();
                         // 필요 시 userData 넘겨주기
                     });

    // connect
    QObject::connect(loginWindow, &LoginMainWindow::loginSuccess, [&] (const QJsonObject& userData) {
        clientMainWindow->setUser(userData);
        clientMainWindow->show();

    });

    loginWindow->show();



    return a.exec();
}
