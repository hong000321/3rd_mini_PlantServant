#ifndef JOINMEMBERMAINWINDOW_H
#define JOINMEMBERMAINWINDOW_H

#include <QMainWindow>
#include "ui_JoinMemberMainWindow.h"
#include "controllers/ClientSocket.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class JoinMemberMainWindow;  // MainJoinMember 변경
}
QT_END_NAMESPACE

class JoinMemberMainWindow : public QMainWindow  // MainWindow로 변경
{
    Q_OBJECT

public:
    JoinMemberMainWindow(QWidget *parent = nullptr, ClientSocket* client_socket = nullptr);
    ~JoinMemberMainWindow();
    Ui::JoinMemberMainWindow *ui;

private slots:
    void on_pushButton_register_clicked();

private:
    ClientSocket* socketclient_;

    void onRegistrationResponse(bool success, const QJsonObject &userData);
};

#endif // JOINMEMBERMAINWINDOW_H
