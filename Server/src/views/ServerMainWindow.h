#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include "ServerConfig.h"
#include "controllers/SocketServer.h"
#include "controllers/FileTransferServer.h"  // 추가
#include "controllers/ProtocolController.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerMainWindow;
}
QT_END_NAMESPACE

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();
    void testPrint();

private slots:
    // JSON 서버 관련 슬롯들
    void onJsonServerStarted(const QHostAddress &address, quint16 port);
    void onJsonServerStopped();
    void onClientConnected(const QString &clientId, const QHostAddress &address);
    void onClientDisconnected(const QString &clientId);
    void onUserLoggedIn(const QString &userId, const QString &clientId);
    void onUserLoggedOut(const QString &userId);
    void onJsonServerError(const QString &error);

    // 파일 서버 관련 슬롯들
    void onFileServerStarted(const QHostAddress &address, quint16 port);
    void onFileServerStopped();
    void onFileClientConnected(const QString &connectionId, const QHostAddress &address);
    void onFileClientDisconnected(const QString &connectionId);
    void onFileTransferCompleted(const QString &connectionId, const QString &fileName);
    void onFileTransferFailed(const QString &connectionId, const QString &error);
    void onFileServerError(const QString &error);

    void updateServerStatus();

    void on_Run_pushButton_clicked();

    void on_pushButton_enroll_clicked();

    void on_pushButton_del_clicked();

private:
    Ui::ServerMainWindow *ui_;
    ServerConfig *config_;

    // 서버 관련 멤버들
    SocketServer *socketServer_;
    FileTransferServer *fileTransferServer_;  // 추가
    ProtocolController *protocolController_;
    QTimer *statusTimer_;
    bool isJsonServerRunning_;   // 이름 변경
    bool isFileServerRunning_;   // 추가

    void initializeServers();    // 이름 변경
    void setupServerConnections();
    void logMessage(const QString &message);
    void updateUI();

    // 서버 시작/중지 메서드
    bool startServers();
    void stopServers();
};

#endif // SERVERMAINWINDOW_H
