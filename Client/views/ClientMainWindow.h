#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include "controllers/ClientSocket.h"
#include "models/service/UserService.h"
#include "models/entity/user.h"
#include "models/service/ChatService.h"
#include "models/service/PostService.h"
#include "models/entity/Post.h"
#include "LoginMainWindow.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ClientMainWindow;
}
QT_END_NAMESPACE

class ClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientMainWindow(ClientSocket* socket, UserService* userService, QWidget* parent = nullptr);
    ~ClientMainWindow();

    void setUser(const QJsonObject& userData);

private slots:

    void on_button_myplant_clicked();

    void on_button_writepost_clicked();

    void on_button_back3_clicked();

    void on_button_back2_clicked();

    void on_button_back_clicked();


    void on_button_save_clicked();

    void on_button_file_clicked();

private:
    Ui::ClientMainWindow *ui;
    ClientSocket* socket_;
    LoginMainWindow* loginMainWindow_;

    UserService* userService_;
    ChatService* chatService_;
    PostService* postService_;

    User currentUser_;
    id_t chatRoomId_ = 1; //전체채팅방 ID고정값
    QString imageBase64_;
};
#endif // CLIENTMAINWINDOW_H
