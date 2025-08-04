#ifndef JOINMEMBERMAINWINDOW_H
#define JOINMEMBERMAINWINDOW_H

#include <QMainWindow>
#include "ui_JoinMemberMainWindow.h"
#include "models/service/UserService.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class JoinMemberMainWindow;
}
QT_END_NAMESPACE

class JoinMemberMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit JoinMemberMainWindow(QWidget *parent = nullptr, UserService* userService = nullptr);
    ~JoinMemberMainWindow();
    Ui::JoinMemberMainWindow *ui;

private slots:
    void on_pushButton_register_clicked();
    void onRegistrationSuccess(const QJsonObject& userData);
    void onRegistrationFailed(const QString& reason);

private:
    UserService* userService_;
};

#endif // JOINMEMBERMAINWINDOW_H
