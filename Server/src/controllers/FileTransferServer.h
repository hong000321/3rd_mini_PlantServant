#ifndef FILETRANSFERSERVER_H
#define FILETRANSFERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include <QDir>
#include "FileTransfer.h"

class FileTransferConnection : public QObject
{
    Q_OBJECT

public:
    explicit FileTransferConnection(QTcpSocket *socket, QObject *parent = nullptr);
    ~FileTransferConnection();

    QString connectionId() const { return connectionId_; }
    QHostAddress clientAddress() const;
    quint16 clientPort() const;
    bool isConnected() const;

    // 파일 전송
    bool sendFile(const QString &filePath);

signals:
    void fileTransferCompleted(const QString &connectionId, const QString &fileName);
    void fileTransferFailed(const QString &connectionId, const QString &error);
    void connectionDisconnected(const QString &connectionId);
    void fileRequested(const QString &connectionId, const QString &fileName);

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);

private:
    QTcpSocket *socket_;
    QString connectionId_;
    FileTransfer *fileTransfer_;
    QByteArray buffer_;

    void processFileRequest();
    void generateConnectionId();
};

class FileTransferServer : public QObject
{
    Q_OBJECT

public:
    explicit FileTransferServer(QObject *parent = nullptr);
    ~FileTransferServer();

    // 서버 제어
    bool startServer(const QHostAddress &address = QHostAddress::Any, quint16 port = 5115);
    bool startServer(quint16 port = 5115);
    void stopServer();
    bool isListening() const;

    // 파일 서비스 디렉토리 설정
    void setImageDirectory(const QString &imagePath);
    QString imageDirectory() const { return m_imageDirectory; }

    // 서버 정보
    QHostAddress serverAddress() const;
    quint16 serverPort() const;
    QString getServerInfo() const;
    int getConnectionCount() const;

signals:
    void serverStarted(const QHostAddress &address, quint16 port);
    void serverStopped();
    void clientConnected(const QString &connectionId, const QHostAddress &address);
    void clientDisconnected(const QString &connectionId);
    void fileTransferCompleted(const QString &connectionId, const QString &fileName);
    void fileTransferFailed(const QString &connectionId, const QString &error);
    void errorOccurred(const QString &error);

private slots:
    void onNewConnection();
    void onConnectionDisconnected(const QString &connectionId);
    void onFileRequested(const QString &connectionId, const QString &fileName);
    void onFileTransferCompleted(const QString &connectionId, const QString &fileName);
    void onFileTransferFailed(const QString &connectionId, const QString &error);

private:
    QTcpServer *m_server;
    QMap<QString, FileTransferConnection*> m_connections;
    QString m_imageDirectory;

    void setupConnections();
    QString getFullImagePath(const QString &fileName) const;
    bool isValidImageFile(const QString &fileName) const;
};

#endif // FILETRANSFERSERVER_H
