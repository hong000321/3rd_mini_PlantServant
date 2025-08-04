#include "ServerMainWindow.h"
#include "ui_server_main_window.h"
#include "views/EnrollAdminWindow.h"
#include "views/TestWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QHostAddress>
#include <QDir>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerMainWindow)
    , socketServer(nullptr)
    , fileTransferServer(nullptr)
    , protocolController(nullptr)
    , statusTimer(new QTimer(this))
    , isJsonServerRunning(false)
    , isFileServerRunning(false)
{
    c = ServerConfig::getInstance();
    ui->setupUi(this);

    // UI 초기화
    ui->lineEdit_user->setText(c->userFilePath);
    ui->lineEdit_product->setText(c->productFilePath);
    ui->lineEdit_order->setText(c->orderFilePath);
    ui->lineEdit_orderItem->setText(c->orderItemFilePath);
    ui->lineEdit_chatUnit->setText(c->chatFilePath);
    ui->lineEdit_chatRoom->setText(c->chatRoomFilePath);
    ui->lineEdit_log->setText(c->logFilePath);

    // 서버들 초기화
    initializeServers();

    // 상태 업데이트 타이머 설정
    connect(statusTimer, &QTimer::timeout, this, &ServerMainWindow::updateServerStatus);
    statusTimer->start(5000); // 5초마다 상태 업데이트
    this->resize(600,400);
    testPrint();
    logMessage("=== PlantServant 서버 애플리케이션이 시작되었습니다 ===");
}

ServerMainWindow::~ServerMainWindow()
{
    stopServers();
    delete ui;
}

void ServerMainWindow::initializeServers()
{
    // JSON 서버 생성 (5105 포트)
    socketServer = new SocketServer(this);

    // 파일 전송 서버 생성 (5115 포트)
    fileTransferServer = new FileTransferServer(this);
    fileTransferServer->setImageDirectory("./../../data/product");

    // ProtocolController 생성 및 초기화
    protocolController = new ProtocolController(this);
    protocolController->setSocketServer(socketServer);

    bool initSuccess = protocolController->initialize(
        c->userFilePath,
        c->productFilePath,
        c->orderFilePath,
        c->orderItemFilePath,
        c->chatRoomFilePath,
        c->chatFilePath
        );

    QString currentPath = QDir::currentPath();
    qDebug() << "현재 작업 디렉토리:" << currentPath;
    qDebug() << "이미지 디렉토리:" << fileTransferServer->imageDirectory();
    qDebug() << c->userFilePath << Qt::endl <<
        c->productFilePath << Qt::endl <<
        c->orderFilePath << Qt::endl <<
        c->orderItemFilePath << Qt::endl <<
        c->chatRoomFilePath << Qt::endl <<
        c->chatFilePath;

    if (!initSuccess) {
        logMessage("⚠️ ProtocolController 초기화 실패 - 일부 데이터 파일을 찾을 수 없습니다.");
    } else {
        logMessage("✅ ProtocolController 초기화 완료");
    }

    setupServerConnections();
}

void ServerMainWindow::setupServerConnections()
{
    // JSON 서버 시그널 연결
    connect(socketServer, &SocketServer::serverStarted,
            this, &ServerMainWindow::onJsonServerStarted);
    connect(socketServer, &SocketServer::serverStopped,
            this, &ServerMainWindow::onJsonServerStopped);
    connect(socketServer, &SocketServer::clientConnected,
            this, &ServerMainWindow::onClientConnected);
    connect(socketServer, &SocketServer::clientDisconnected,
            this, &ServerMainWindow::onClientDisconnected);
    connect(socketServer, &SocketServer::userLoggedIn,
            this, &ServerMainWindow::onUserLoggedIn);
    connect(socketServer, &SocketServer::userLoggedOut,
            this, &ServerMainWindow::onUserLoggedOut);
    connect(socketServer, &SocketServer::errorOccurred,
            this, &ServerMainWindow::onJsonServerError);

    // 파일 서버 시그널 연결
    connect(fileTransferServer, &FileTransferServer::serverStarted,
            this, &ServerMainWindow::onFileServerStarted);
    connect(fileTransferServer, &FileTransferServer::serverStopped,
            this, &ServerMainWindow::onFileServerStopped);
    connect(fileTransferServer, &FileTransferServer::clientConnected,
            this, &ServerMainWindow::onFileClientConnected);
    connect(fileTransferServer, &FileTransferServer::clientDisconnected,
            this, &ServerMainWindow::onFileClientDisconnected);
    connect(fileTransferServer, &FileTransferServer::fileTransferCompleted,
            this, &ServerMainWindow::onFileTransferCompleted);
    connect(fileTransferServer, &FileTransferServer::fileTransferFailed,
            this, &ServerMainWindow::onFileTransferFailed);
    connect(fileTransferServer, &FileTransferServer::errorOccurred,
            this, &ServerMainWindow::onFileServerError);

    // JSON 서버와 ProtocolController 연결
    connect(socketServer, &SocketServer::jsonDataReceived,
            protocolController, &ProtocolController::onClientMessageReceived);

    // ProtocolController에서 JSON 서버로의 데이터 전송
    connect(protocolController, &ProtocolController::sendToClient,
            socketServer, QOverload<const QString&, const QJsonObject&>::of(&SocketServer::sendJsonToClient));
    connect(protocolController, &ProtocolController::sendToUser,
            socketServer, QOverload<const QString&, const QJsonObject&>::of(&SocketServer::sendJsonToUser));
    connect(protocolController, &ProtocolController::broadcastToClients,
            socketServer, QOverload<const QJsonObject&>::of(&SocketServer::broadcastJson));
    connect(protocolController, &ProtocolController::broadcastToUsers,
            socketServer, &SocketServer::broadcastToUsers);

    // ProtocolController 에러 연결
    connect(protocolController, &ProtocolController::errorOccurred,
            this, [this](const QString &error) {
                logMessage("🔥 프로토콜 에러: " + error);
            });
}

bool ServerMainWindow::startServers()
{
    QString ipAddress = ui->IP_lineEdit->text().trimmed();

    if (ipAddress.isEmpty()) {
        ipAddress = "127.0.0.1";
    }

    QHostAddress address(ipAddress);
    if (address.isNull()) {
        QMessageBox::warning(this, "오류", "유효하지 않은 IP 주소입니다.");
        return false;
    }

    logMessage(QString("🚀 서버들 시작 시도... %1").arg(ipAddress));

    // JSON 서버 시작 (5105 포트)
    if (!socketServer->startServer(address, 5105)) {
        logMessage("❌ JSON 서버 시작 실패 (포트 5105)");
        return false;
    }

    // 파일 전송 서버 시작 (5115 포트)
    if (!fileTransferServer->startServer(address, 5115)) {
        logMessage("❌ 파일 전송 서버 시작 실패 (포트 5115)");
        socketServer->stopServer(); // JSON 서버도 중지
        return false;
    }

    return true;
}

void ServerMainWindow::stopServers()
{
    if (socketServer && isJsonServerRunning) {
        socketServer->stopServer();
    }
    if (fileTransferServer && isFileServerRunning) {
        fileTransferServer->stopServer();
    }
}

// JSON 서버 관련 슬롯들
void ServerMainWindow::onJsonServerStarted(const QHostAddress &address, quint16 port)
{
    isJsonServerRunning = true;
    logMessage(QString("✅ JSON 서버가 시작되었습니다: %1:%2").arg(address.toString()).arg(port));
    updateUI();
}

void ServerMainWindow::onJsonServerStopped()
{
    isJsonServerRunning = false;
    logMessage("🛑 JSON 서버가 중지되었습니다.");
    updateUI();
}

void ServerMainWindow::onClientConnected(const QString &clientId, const QHostAddress &address)
{
    logMessage(QString("🔗 JSON 클라이언트 연결: %1 (%2)").arg(clientId).arg(address.toString()));
    updateUI();
}

void ServerMainWindow::onClientDisconnected(const QString &clientId)
{
    logMessage(QString("❌ JSON 클라이언트 연결 해제: %1").arg(clientId));
    updateUI();
}

void ServerMainWindow::onUserLoggedIn(const QString &userId, const QString &clientId)
{
    logMessage(QString("👤 사용자 로그인: ID=%1, Client=%2").arg(userId, clientId));
    updateUI();
}

void ServerMainWindow::onUserLoggedOut(const QString &userId)
{
    logMessage(QString("👋 사용자 로그아웃: ID=%1").arg(userId));
    updateUI();
}

void ServerMainWindow::onJsonServerError(const QString &error)
{
    logMessage("🔥 JSON 서버 오류: " + error);
}

// 파일 서버 관련 슬롯들
void ServerMainWindow::onFileServerStarted(const QHostAddress &address, quint16 port)
{
    isFileServerRunning = true;
    logMessage(QString("✅ 파일 전송 서버가 시작되었습니다: %1:%2").arg(address.toString()).arg(port));
    updateUI();
}

void ServerMainWindow::onFileServerStopped()
{
    isFileServerRunning = false;
    logMessage("🛑 파일 전송 서버가 중지되었습니다.");
    updateUI();
}

void ServerMainWindow::onFileClientConnected(const QString &connectionId, const QHostAddress &address)
{
    logMessage(QString("📁 파일 클라이언트 연결: %1 (%2)").arg(connectionId).arg(address.toString()));
}

void ServerMainWindow::onFileClientDisconnected(const QString &connectionId)
{
    logMessage(QString("📁 파일 클라이언트 연결 해제: %1").arg(connectionId));
}

void ServerMainWindow::onFileTransferCompleted(const QString &connectionId, const QString &fileName)
{
    logMessage(QString("📤 파일 전송 완료: %1 → %2").arg(fileName).arg(connectionId));
}

void ServerMainWindow::onFileTransferFailed(const QString &connectionId, const QString &error)
{
    logMessage(QString("📤 파일 전송 실패: %1 - %2").arg(connectionId).arg(error));
}

void ServerMainWindow::onFileServerError(const QString &error)
{
    logMessage("🔥 파일 서버 오류: " + error);
}

void ServerMainWindow::updateServerStatus()
{
    if ((isJsonServerRunning || isFileServerRunning)) {
        QString statusInfo = "📊 서버 상태";

        if (isJsonServerRunning && socketServer) {
            int clientCount = socketServer->getClientCount();
            int userCount = socketServer->getLoggedInUserCount();
            statusInfo += QString(" - JSON: 연결 %1개, 로그인 %2명").arg(clientCount).arg(userCount);
        }

        if (isFileServerRunning && fileTransferServer) {
            int fileConnections = fileTransferServer->getConnectionCount();
            statusInfo += QString(" - 파일: 연결 %1개").arg(fileConnections);
        }

        // 상세 정보 추가 (5분마다)
        static int statusCounter = 0;
        statusCounter++;
        if (statusCounter >= 60) { // 5초 * 60 = 5분
            statusCounter = 0;
            if (socketServer) {
                QStringList connectedUsers = socketServer->getLoggedInUsers();
                if (!connectedUsers.isEmpty()) {
                    statusInfo += QString(" [사용자: %1]").arg(connectedUsers.join(", "));
                }
            }
        }

        logMessage(statusInfo);
    }
}

void ServerMainWindow::updateUI()
{
    // 제목 업데이트
    QString title = "BlueButton Server";
    // if (isJsonServerRunning || isFileServerRunning) {
    //     title += " - Running";
    //     if (isJsonServerRunning && socketServer) {
    //         int userCount = socketServer->getLoggedInUserCount();
    //         title += QString(" [JSON:%1:%2, Users:%3]")
    //                      .arg(socketServer->serverAddress().toString())
    //                      .arg(socketServer->serverPort())
    //                      .arg(userCount);
    //     }
    //     if (isFileServerRunning && fileTransferServer) {
    //         title += QString(" [File:%1:%2]")
    //         .arg(fileTransferServer->serverAddress().toString())
    //             .arg(fileTransferServer->serverPort());
    //     }
    // }
    setWindowTitle(title);
}

void ServerMainWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);

    ui->Log_textBrowser->append(logEntry);
    qDebug() << logEntry;

    // 자동 스크롤
    QTextCursor cursor = ui->Log_textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->Log_textBrowser->setTextCursor(cursor);
}

void ServerMainWindow::testPrint()
{
    qDebug() << tr("hello world") << Qt::endl;
    qDebug() << tr("안녕하세요") << Qt::endl;
}



void ServerMainWindow::on_pushButton_enroll_clicked()
{
    if (!isJsonServerRunning) {
        QMessageBox::information(this, "알림", "관리자를 등록하려면 먼저 서버를 시작해주세요.");
        return;
    }

    EnrollAdminWindow *enroll_ui = new EnrollAdminWindow(protocolController, this);
    enroll_ui->show();
}

void ServerMainWindow::on_pushButton_del_clicked()
{
    // 관리자 삭제 기능 구현
    if (!isJsonServerRunning) {
        QMessageBox::information(this, "알림", "서버가 실행 중이 아닙니다.");
        return;
    }

    // TODO: 관리자 삭제 창 구현
    QMessageBox::information(this, "알림", "관리자 삭제 기능은 추후 구현 예정입니다.");
}


void ServerMainWindow::on_Run_pushButton_clicked()
{
    if (!isJsonServerRunning && !isFileServerRunning) {
        // 서버들 시작
        if (startServers()) {
            ui->Run_pushButton->setText("Stop");
            ui->IP_lineEdit->setEnabled(false);
            ui->Port_lineEdit_2->setEnabled(false);
        } else {
            QMessageBox::critical(this, "오류", "서버 시작에 실패했습니다.");
        }
    } else {
        // 서버들 중지
        logMessage("🛑 서버들 중지 중...");
        stopServers();
        ui->Run_pushButton->setText("Run");
        ui->IP_lineEdit->setEnabled(true);
        ui->Port_lineEdit_2->setEnabled(true);
    }
}

