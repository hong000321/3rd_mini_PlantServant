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
    socket->connectToServer("192.168.2.180",54321);
    UserService* userService = new UserService(socket); // 로그인, 회원가입, 로그아웃 담당
    ClientMainWindow* clientMainWindow = new ClientMainWindow();
    LoginMainWindow* login = new LoginMainWindow(nullptr, userService);
    login->show();



    return a.exec();
}
