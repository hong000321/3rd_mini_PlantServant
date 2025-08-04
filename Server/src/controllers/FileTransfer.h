#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>
#include <QDataStream>
#include <QFileInfo>

class FileTransfer : public QObject
{
    Q_OBJECT

public:
    explicit FileTransfer(QTcpSocket *socket, QObject *parent = nullptr);
    ~FileTransfer();

    // 파일 전송 (클라이언트용)
    bool sendFile(const QString &filePath);

    // 파일 수신 처리 (서버용)
    void processFileData(const QByteArray &data);

    // 상태 확인
    bool isSending() const { return m_sendFile != nullptr; }
    bool isReceiving() const { return m_receiveFile != nullptr; }
    bool isTransferring() const { return isSending() || isReceiving(); }

    // 전송 중단
    void cancelTransfer();

signals:
    // 파일 전송 시그널
    void fileSendStarted(const QString &fileName, qint64 totalSize);
    void fileSendProgress(qint64 bytesSent, qint64 totalBytes);
    void fileSendCompleted(const QString &fileName);
    void fileSendFailed(const QString &error);

    // 파일 수신 시그널
    void fileReceiveStarted(const QString &fileName, qint64 totalSize);
    void fileReceiveProgress(qint64 bytesReceived, qint64 totalBytes);
    void fileReceiveCompleted(const QString &fileName);
    void fileReceiveFailed(const QString &error);

private slots:
    void sendNextBlock();

private:
    QTcpSocket *m_socket;  // 외부에서 주입받은 소켓 (소유하지 않음)

    // 파일 전송용
    QFile *m_sendFile;
    QTimer *m_sendTimer;
    QString m_sendFileName;
    qint64 m_totalSendBytes;
    qint64 m_sentBytes;
    qint64 m_blockSize;

    // 파일 수신용
    QFile *m_receiveFile;
    QString m_receiveFileName;
    qint64 m_totalReceiveBytes;
    qint64 m_receivedBytes;
    QByteArray m_receiveBuffer;

    enum ReceiveState {
        WaitingForHeader,
        WaitingForData
    } m_receiveState;

    // 유틸리티
    void cleanupSend();
    void cleanupReceive();
    QByteArray createFileHeader(const QString &fileName, qint64 fileSize);
    bool parseFileHeader(const QByteArray &data, QString &fileName, qint64 &fileSize);
};

#endif // FILETRANSFER_H
