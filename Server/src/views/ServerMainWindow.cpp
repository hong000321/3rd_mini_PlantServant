#include "ServerMainWindow.h"
#include "ui_server_main_window.h"
#include "views/EnrollAdminWindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QHostAddress>
#include <QDir>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::ServerMainWindow)
    , socketServer_(nullptr)
    , protocolController_(nullptr)
    , restServer_(nullptr)
    , statusTimer_(new QTimer(this))
    , isJsonServerRunning_(false)
{
    config_ = ServerConfig::getInstance();
    ui_->setupUi(this);

    // UI 초기화 (product → plant로 변경)
    ui_->lineEdit_user->setText(config_->userFilePath);
    ui_->lineEdit_plant->setText(config_->plantFilePath);
    ui_->lineEdit_post->setText(config_->postFilePath);
    ui_->lineEdit_chatUnit->setText(config_->chatFilePath);
    ui_->lineEdit_chatRoom->setText(config_->chatRoomFilePath);
    ui_->lineEdit_log->setText(config_->logFilePath);

    // 서버들 초기화
    initializeServers();
    sensorDB_ = SensorDB::getInstance();
    sensorDB_->initializeDatabase();
    sensorDB_->createTables();

    // 상태 업데이트 타이머 설정
    connect(statusTimer_, &QTimer::timeout, this, &ServerMainWindow::updateServerStatus);
    statusTimer_->start(5000); // 5초마다 상태 업데이트
    this->resize(600,400);
    testPrint();
    logMessage("=== PlantServant 서버 애플리케이션이 시작되었습니다 ===");
}

ServerMainWindow::~ServerMainWindow()
{
    stopServers();
    delete ui_;
}

void ServerMainWindow::initializeServers()
{
    // JSON 서버 생성 (5105 포트)
    socketServer_ = new SocketServer(this);

    // ProtocolController 생성 및 초기화
    protocolController_ = new ProtocolController(this);
    protocolController_->setSocketServer(socketServer_);

    bool initSuccess = protocolController_->initialize(
        config_->userFilePath,
        config_->plantFilePath,
        config_->postFilePath,
        config_->chatRoomFilePath,
        config_->chatFilePath
        );

    restServer_ = new HttpRestServer(this);

    QString currentPath = QDir::currentPath();
    qDebug() << "현재 작업 디렉토리:" << currentPath;
    qDebug() << config_->userFilePath << Qt::endl <<
        config_->plantFilePath << Qt::endl <<
        config_->postFilePath << Qt::endl <<
        config_->chatRoomFilePath << Qt::endl <<
        config_->chatFilePath;

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
    connect(socketServer_, &SocketServer::serverStarted,
            this, &ServerMainWindow::onJsonServerStarted);
    connect(socketServer_, &SocketServer::serverStopped,
            this, &ServerMainWindow::onJsonServerStopped);
    connect(socketServer_, &SocketServer::clientConnected,
            this, &ServerMainWindow::onClientConnected);
    connect(socketServer_, &SocketServer::clientDisconnected,
            this, &ServerMainWindow::onClientDisconnected);
    connect(socketServer_, &SocketServer::userLoggedIn,
            this, &ServerMainWindow::onUserLoggedIn);
    connect(socketServer_, &SocketServer::userLoggedOut,
            this, &ServerMainWindow::onUserLoggedOut);
    connect(socketServer_, &SocketServer::errorOccurred,
            this, &ServerMainWindow::onJsonServerError);

    // JSON 서버와 ProtocolController 연결
    connect(socketServer_, &SocketServer::jsonDataReceived,
            protocolController_, &ProtocolController::onClientMessageReceived);

    // ProtocolController에서 JSON 서버로의 데이터 전송
    connect(protocolController_, &ProtocolController::sendToClient,
            socketServer_, QOverload<const QString&, const QJsonObject&>::of(&SocketServer::sendJsonToClient));
    connect(protocolController_, &ProtocolController::sendToUser,
            socketServer_, QOverload<const QString&, const QJsonObject&>::of(&SocketServer::sendJsonToUser));
    connect(protocolController_, &ProtocolController::broadcastToClients,
            socketServer_, QOverload<const QJsonObject&>::of(&SocketServer::broadcastJson));
    connect(protocolController_, &ProtocolController::broadcastToUsers,
            socketServer_, &SocketServer::broadcastToUsers);

    // ProtocolController 에러 연결
    connect(protocolController_, &ProtocolController::errorOccurred,
            this, [this](const QString &error) {
                logMessage("🔥 프로토콜 에러: " + error);
            });
}

bool ServerMainWindow::startServers()
{
    QString ipAddress = ui_->IP_lineEdit->text().trimmed();

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
    quint16 port = static_cast<quint16>(ui_->Port_lineEdit_2->text().toInt());
    if (!socketServer_->startServer(address, port)) {
        logMessage(QString("❌ JSON 서버 시작 실패 (포트 %1)").arg(port));
        return false;
    }

    // HTTP REST Server 시작 8080포트
    restServer_->start(8080);

    return true;
}

void ServerMainWindow::stopServers()
{
    if (socketServer_ && isJsonServerRunning_) {
        socketServer_->stopServer();
    }
}

// JSON 서버 관련 슬롯들
void ServerMainWindow::onJsonServerStarted(const QHostAddress &address, quint16 port)
{
    isJsonServerRunning_ = true;
    logMessage(QString("✅ JSON 서버가 시작되었습니다: %1:%2").arg(address.toString()).arg(port));
    updateUI();
}

void ServerMainWindow::onJsonServerStopped()
{
    isJsonServerRunning_ = false;
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

void ServerMainWindow::updateServerStatus()
{
    if ((isJsonServerRunning_)) {
        QString statusInfo = "📊 서버 상태";

        if (isJsonServerRunning_ && socketServer_) {
            int clientCount = socketServer_->getClientCount();
            int userCount = socketServer_->getLoggedInUserCount();
            statusInfo += QString(" - JSON: 연결 %1개, 로그인 %2명").arg(clientCount).arg(userCount);
        }

        // 상세 정보 추가 (5분마다)
        static int statusCounter = 0;
        statusCounter++;
        if (statusCounter >= 60) { // 5초 * 60 = 5분
            statusCounter = 0;
            if (socketServer_) {
                QStringList connectedUsers = socketServer_->getLoggedInUsers();
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
    // if (isJsonServerRunning_ ) {
    //     title += " - Running";
    //     if (isJsonServerRunning_ && socketServer_) {
    //         int userCount = socketServer_->getLoggedInUserCount();
    //         title += QString(" [JSON:%1:%2, Users:%3]")
    //                      .arg(socketServer_->serverAddress().toString())
    //                      .arg(socketServer_->serverPort())
    //                      .arg(userCount);
    //     }
    // }
    setWindowTitle(title);
}

void ServerMainWindow::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp).arg(message);

    ui_->Log_textBrowser->append(logEntry);
    qDebug() << logEntry;

    // 자동 스크롤
    QTextCursor cursor = ui_->Log_textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui_->Log_textBrowser->setTextCursor(cursor);
}

void ServerMainWindow::testPrint()
{
    qDebug() << tr("hello world") << Qt::endl;
    qDebug() << tr("안녕하세요") << Qt::endl;
}



void ServerMainWindow::on_pushButton_enroll_clicked()
{
    if (!isJsonServerRunning_) {
        QMessageBox::information(this, "알림", "관리자를 등록하려면 먼저 서버를 시작해주세요.");
        return;
    }

    EnrollAdminWindow *enroll_ui_ = new EnrollAdminWindow(protocolController_, this);
    enroll_ui_->show();
}

void ServerMainWindow::on_pushButton_del_clicked()
{
    // 관리자 삭제 기능 구현
    if (!isJsonServerRunning_) {
        QMessageBox::information(this, "알림", "서버가 실행 중이 아닙니다.");
        return;
    }

    // TODO: 관리자 삭제 창 구현
    QMessageBox::information(this, "알림", "관리자 삭제 기능은 추후 구현 예정입니다.");
}


void ServerMainWindow::on_Run_pushButton_clicked()
{
    if (!isJsonServerRunning_) {
        // 서버들 시작
        if (startServers()) {
            ui_->Run_pushButton->setText("Stop");
            ui_->IP_lineEdit->setEnabled(false);
            ui_->Port_lineEdit_2->setEnabled(false);
        } else {
            QMessageBox::critical(this, "오류", "서버 시작에 실패했습니다.");
        }
    } else {
        // 서버들 중지
        logMessage("🛑 서버들 중지 중...");
        stopServers();
        ui_->Run_pushButton->setText("Run");
        ui_->IP_lineEdit->setEnabled(true);
        ui_->Port_lineEdit_2->setEnabled(true);
    }
}
