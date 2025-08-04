#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include "controllers/ClientSocket.h"
#include "models/service/UserService.h"
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
    explicit ClientMainWindow(ClientSocket* socket = nullptr, QWidget* parent = nullptr);
    ~ClientMainWindow();

private slots:

    void on_button_myplant_clicked();

    void on_button_writepost_clicked();

    void on_button_back3_clicked();

    void on_button_back2_clicked();

    void on_button_back_clicked();

private:
    Ui::ClientMainWindow *ui;
    ClientSocket* socket_;
    UserService* userService_;
    LoginMainWindow* loginMainWindow_;
};
#endif // CLIENTMAINWINDOW_H
