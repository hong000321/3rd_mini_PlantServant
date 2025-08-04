#ifndef ENROLLADMINWINDOW_H
#define ENROLLADMINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "controllers/ProtocolController.h"
#include "models/entity/user/User.h"

namespace Ui {
class EnrollAdminWindow;
}

class EnrollAdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EnrollAdminWindow(ProtocolController *protocolController, QWidget *parent = nullptr);
    ~EnrollAdminWindow();

private slots:
    void on_pushButton_register_clicked();
    void on_lineEdit_id_textChanged(const QString &text);
    void on_lineEdit_email_textChanged(const QString &text);

private:
    Ui::EnrollAdminWindow *ui_;
    ProtocolController *protocolController_;

    bool validateInput();
    void clearForm();
    void showMessage(const QString &title, const QString &message, bool isError = false);
};

#endif // ENROLLADMINWINDOW_H
