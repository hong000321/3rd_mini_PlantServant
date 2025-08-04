#include "FileTransferServer.h"
#include <QDebug>
#include <QUuid>
#include <QFileInfo>
#include <QDataStream>

// FileTransferConnection 구현
FileTransferConnection::FileTransferConnection(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_fileTransfer(nullptr)
{
    generateConnectionId();
    m_socket->setParent(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &FileTransferConnection::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &FileTransferConnection::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &FileTransferConnection::onSocketError);

    // FileTransfer 인스턴스 생성
    m_fileTransfer = new FileTransfer(m_socket, this);

    // FileTransfer 시그널 연결
    connect(m_fileTransfer, &FileTransfer::fileSendCompleted, this,
            [this](const QString &fileName) {
                emit fileTransferCompleted(m_connectionId, fileName);
            });

    connect(m_fileTransfer, &FileTransfer::fileSendFailed, this,
            [this](const QString &error) {
                emit fileTransferFailed(m_connectionId, error);
            });

    qDebug() << "FileTransfer client connected:" << m_connectionId << "from" << clientAddress().toString();
}

FileTransferConnection::~FileTransferConnection()
{
    if (m_fileTransfer) {
        delete m_fileTransfer;
    }

    if (m_socket && m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
    }
}

void FileTransferConnection::generateConnectionId()
{
    m_connectionId = QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

QHostAddress FileTransferConnection::clientAddress() const
{
    return m_socket ? m_socket->peerAddress() : QHostAddress();
}

quint16 FileTransferConnection::clientPort() const
{
    return m_socket ? m_socket->peerPort() : 0;
}

bool FileTransferConnection::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

bool FileTransferConnection::sendFile(const QString &filePath)
{
    if (!m_fileTransfer) {
        emit fileTransferFailed(m_connectionId, "FileTransfer not initialized");
        return false;
    }

    return m_fileTransfer->sendFile(filePath);
}

void FileTransferConnection::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    processFileRequest();
}

void FileTransferConnection::processFileRequest()
{
    // 간단한 텍스트 기반 파일 요청 프로토콜
    // 클라이언트가 파일명을 텍스트로 전송하면 해당 파일을 전송
    while (m_buffer.contains('\n')) {
        int newlineIndex = m_buffer.indexOf('\n');
        QByteArray requestLine = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);

        QString fileName = QString::fromUtf8(requestLine).trimmed();
        if (!fileName.isEmpty()) {
            qDebug() << "File requested:" << fileName << "from" << m_connectionId;
            emit fileRequested(m_connectionId, fileName);
        }
    }
}

void FileTransferConnection::onDisconnected()
{
    qDebug() << "FileTransfer client disconnected:" << m_connectionId;
    emit connectionDisconnected(m_connectionId);
}

void FileTransferConnection::onSocketError(QAbstractSocket::SocketError error)
{
    QString errorString = m_socket->errorString();
    qDebug() << "FileTransfer client socket error for" << m_connectionId << ":" << errorString;
    emit fileTransferFailed(m_connectionId, errorString);
}

// FileTransferServer 구현
FileTransferServer::FileTransferServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_imageDirectory("./images/products")  // 기본 이미지 디렉토리
{
    setupConnections();
}

FileTransferServer::~FileTransferServer()
{
    stopServer();
}

void FileTransferServer::setupConnections()
{
    connect(m_server, &QTcpServer::newConnection, this, &FileTransferServer::onNewConnection);
}

bool FileTransferServer::startServer(const QHostAddress &address, quint16 port)
{
    if (m_server->isListening()) {
        emit errorOccurred("FileTransfer server is already listening");
        return false;
    }

    bool success = m_server->listen(address, port);

    if (success) {
        qDebug() << "FileTransfer server started on" << m_server->serverAddress().toString()
        << ":" << m_server->serverPort();
        emit serverStarted(m_server->serverAddress(), m_server->serverPort());
    } else {
        QString error = QString("Failed to start FileTransfer server: %1").arg(m_server->errorString());
        qDebug() << error;
        emit errorOccurred(error);
    }

    return success;
}

bool FileTransferServer::startServer(quint16 port)
{
    return startServer(QHostAddress::Any, port);
}

void FileTransferServer::stopServer()
{
    if (!m_server->isListening()) {
        return;
    }

    // 모든 연결 종료
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        it.value()->deleteLater();
    }
    m_connections.clear();

    m_server->close();
    qDebug() << "FileTransfer server stopped";
    emit serverStopped();
}

bool FileTransferServer::isListening() const
{
    return m_server->isListening();
}

void FileTransferServer::setImageDirectory(const QString &imagePath)
{
    QDir dir(imagePath);
    if (dir.exists()) {
        m_imageDirectory = dir.absolutePath();
        qDebug() << "Image directory set to:" << m_imageDirectory;
    } else {
        qDebug() << "Warning: Image directory does not exist:" << imagePath;
        m_imageDirectory = imagePath; // 일단 설정은 해둠
    }
}

QHostAddress FileTransferServer::serverAddress() const
{
    return m_server->serverAddress();
}

quint16 FileTransferServer::serverPort() const
{
    return m_server->serverPort();
}

QString FileTransferServer::getServerInfo() const
{
    if (isListening()) {
        return QString("FileTransfer server listening on %1:%2 (%3 connections)")
        .arg(serverAddress().toString())
            .arg(serverPort())
            .arg(getConnectionCount());
    } else {
        return "FileTransfer server is not running";
    }
}

int FileTransferServer::getConnectionCount() const
{
    return m_connections.size();
}

void FileTransferServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        FileTransferConnection *connection = new FileTransferConnection(socket, this);

        connect(connection, &FileTransferConnection::connectionDisconnected,
                this, &FileTransferServer::onConnectionDisconnected);
        connect(connection, &FileTransferConnection::fileRequested,
                this, &FileTransferServer::onFileRequested);
        connect(connection, &FileTransferConnection::fileTransferCompleted,
                this, &FileTransferServer::onFileTransferCompleted);
        connect(connection, &FileTransferConnection::fileTransferFailed,
                this, &FileTransferServer::onFileTransferFailed);

        m_connections[connection->connectionId()] = connection;

        emit clientConnected(connection->connectionId(), connection->clientAddress());
    }
}

void FileTransferServer::onConnectionDisconnected(const QString &connectionId)
{
    if (m_connections.contains(connectionId)) {
        m_connections[connectionId]->deleteLater();
        m_connections.remove(connectionId);
        emit clientDisconnected(connectionId);
    }
}

void FileTransferServer::onFileRequested(const QString &connectionId, const QString &fileName)
{
    FileTransferConnection *connection = m_connections.value(connectionId, nullptr);
    if (!connection) {
        qDebug() << "Connection not found:" << connectionId;
        return;
    }

    // 파일 유효성 검사
    if (!isValidImageFile(fileName)) {
        emit fileTransferFailed(connectionId, "Invalid file type or name: " + fileName);
        return;
    }

    QString fullPath = getFullImagePath(fileName);
    QFileInfo fileInfo(fullPath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit fileTransferFailed(connectionId, "File not found: " + fileName);
        return;
    }

    // 파일 전송 시작
    qDebug() << "Sending file:" << fullPath << "to" << connectionId;
    if (!connection->sendFile(fullPath)) {
        emit fileTransferFailed(connectionId, "Failed to start file transfer: " + fileName);
    }
}

void FileTransferServer::onFileTransferCompleted(const QString &connectionId, const QString &fileName)
{
    qDebug() << "File transfer completed:" << fileName << "to" << connectionId;
    emit fileTransferCompleted(connectionId, fileName);
}

void FileTransferServer::onFileTransferFailed(const QString &connectionId, const QString &error)
{
    qDebug() << "File transfer failed for" << connectionId << ":" << error;
    emit fileTransferFailed(connectionId, error);
}

QString FileTransferServer::getFullImagePath(const QString &fileName) const
{
    QDir imageDir(m_imageDirectory);
    return imageDir.absoluteFilePath(fileName);
}

bool FileTransferServer::isValidImageFile(const QString &fileName) const
{
    // 보안을 위한 기본적인 검증
    if (fileName.contains("..") || fileName.contains("/") || fileName.contains("\\")) {
        return false;
    }

    // 허용된 이미지 확장자
    QStringList allowedExtensions = {"jpg", "jpeg", "png", "gif", "bmp", "webp"};
    QFileInfo fileInfo(fileName);
    QString suffix = fileInfo.suffix().toLower();

    return allowedExtensions.contains(suffix);
}
