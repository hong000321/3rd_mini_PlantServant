#include "FileTransfer.h"
#include <QDebug>
#include <QDataStream>

FileTransfer::FileTransfer(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_sendFile(nullptr)
    , m_sendTimer(new QTimer(this))
    , m_totalSendBytes(0)
    , m_sentBytes(0)
    , m_blockSize(4096)  // 4KB 블록
    , receiveFile_(nullptr)
    , totalReceiveBytes_(0)
    , receivedBytes_(0)
    , receiveState_(WaitingForHeader)
{
    connect(m_sendTimer, &QTimer::timeout, this, &FileTransfer::sendNextBlock);
    m_sendTimer->setSingleShot(true);
}

FileTransfer::~FileTransfer()
{
    cancelTransfer();
}

bool FileTransfer::sendFile(const QString &filePath)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        emit fileSendFailed("Socket not connected");
        return false;
    }

    if (isSending()) {
        emit fileSendFailed("Another file transfer is in progress");
        return false;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        emit fileSendFailed("File not found: " + filePath);
        return false;
    }

    m_sendFile = new QFile(filePath);
    if (!m_sendFile->open(QFile::ReadOnly)) {
        emit fileSendFailed("Cannot open file: " + m_sendFile->errorString());
        delete m_sendFile;
        m_sendFile = nullptr;
        return false;
    }

    m_sendFileName = fileInfo.fileName();
    m_totalSendBytes = fileInfo.size();
    m_sentBytes = 0;

    // 파일 헤더 전송
    QByteArray header = createFileHeader(m_sendFileName, m_totalSendBytes);
    if (m_socket->write(header) != header.size()) {
        emit fileSendFailed("Failed to send file header");
        cleanupSend();
        return false;
    }

    qDebug() << "File send started:" << m_sendFileName << "Size:" << m_totalSendBytes;
    emit fileSendStarted(m_sendFileName, m_totalSendBytes);

    // 첫 번째 블록 전송 시작
    m_sendTimer->start(10);

    return true;
}

void FileTransfer::sendNextBlock()
{
    if (!m_sendFile || m_sendFile->atEnd()) {
        // 전송 완료
        if (m_sendFile) {
            qDebug() << "File send completed:" << m_sendFileName;
            emit fileSendCompleted(m_sendFileName);
            cleanupSend();
        }
        return;
    }

    QByteArray block = m_sendFile->read(m_blockSize);
    qint64 bytesWritten = m_socket->write(block);

    if (bytesWritten == -1) {
        emit fileSendFailed("Socket write error: " + m_socket->errorString());
        cleanupSend();
        return;
    }

    m_sentBytes += bytesWritten;
    emit fileSendProgress(m_sentBytes, m_totalSendBytes);

    // 다음 블록 전송 스케줄링
    if (!m_sendFile->atEnd()) {
        m_sendTimer->start(10);
    } else {
        m_sendTimer->start(10); // 완료 처리
    }
}

void FileTransfer::processFileData(const QByteArray &data)
{
    receiveBuffer_.append(data);

    while (!receiveBuffer_.isEmpty()) {
        if (receiveState_ == WaitingForHeader) {
            // 헤더 크기 확인 (최소 24바이트: 8+8+최소문자열)
            if (receiveBuffer_.size() < 24) {
                break;
            }

            QString fileName;
            qint64 fileSize;

            if (!parseFileHeader(receiveBuffer_, fileName, fileSize)) {
                emit fileReceiveFailed("Invalid file header");
                cleanupReceive();
                return;
            }

            // 헤더 파싱 성공 - 파일 수신 준비
            receiveFileName_ = fileName;
            totalReceiveBytes_ = fileSize;
            receivedBytes_ = 0;

            // 파일 생성
            if (receiveFile_) {
                delete receiveFile_;
            }

            QFileInfo fileInfo(receiveFileName_);
            receiveFile_ = new QFile(fileInfo.fileName());

            if (!receiveFile_->open(QFile::WriteOnly)) {
                emit fileReceiveFailed("Cannot create file: " + receiveFile_->errorString());
                cleanupReceive();
                return;
            }

            receiveState_ = WaitingForData;
            qDebug() << "File receive started:" << receiveFileName_ << "Size:" << totalReceiveBytes_;
            emit fileReceiveStarted(receiveFileName_, totalReceiveBytes_);
        }
        else if (receiveState_ == WaitingForData) {
            // 파일 데이터 수신
            qint64 remainingBytes = totalReceiveBytes_ - receivedBytes_;
            qint64 bytesToWrite = qMin(static_cast<qint64>(receiveBuffer_.size()), remainingBytes);

            if (bytesToWrite <= 0) {
                break;
            }

            QByteArray fileData = receiveBuffer_.left(bytesToWrite);
            receiveBuffer_.remove(0, bytesToWrite);

            qint64 written = receiveFile_->write(fileData);
            if (written != bytesToWrite) {
                emit fileReceiveFailed("File write error: " + receiveFile_->errorString());
                cleanupReceive();
                return;
            }

            receivedBytes_ += written;
            emit fileReceiveProgress(receivedBytes_, totalReceiveBytes_);

            if (receivedBytes_ >= totalReceiveBytes_) {
                // 수신 완료
                qDebug() << "File receive completed:" << receiveFileName_;
                emit fileReceiveCompleted(receiveFileName_);
                cleanupReceive();
                receiveState_ = WaitingForHeader;
            }
        }
    }
}

void FileTransfer::cancelTransfer()
{
    cleanupSend();
    cleanupReceive();
}

void FileTransfer::cleanupSend()
{
    if (m_sendTimer) {
        m_sendTimer->stop();
    }

    if (m_sendFile) {
        m_sendFile->close();
        delete m_sendFile;
        m_sendFile = nullptr;
    }

    m_sendFileName.clear();
    m_totalSendBytes = 0;
    m_sentBytes = 0;
}

void FileTransfer::cleanupReceive()
{
    if (receiveFile_) {
        receiveFile_->close();
        delete receiveFile_;
        receiveFile_ = nullptr;
    }

    receiveFileName_.clear();
    totalReceiveBytes_ = 0;
    receivedBytes_ = 0;
    receiveBuffer_.clear();
}

QByteArray FileTransfer::createFileHeader(const QString &fileName, qint64 fileSize)
{
    QByteArray header;
    QDataStream stream(&header, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    // 매직 넘버 (파일 전송 식별)
    stream << quint32(0xF11E7562);  // "FILE" in hex
    stream << quint64(fileSize);
    stream << fileName;

    // 전체 헤더 크기 추가 (프로토콜 확장성)
    QByteArray fullHeader;
    QDataStream fullStream(&fullHeader, QIODevice::WriteOnly);
    fullStream.setByteOrder(QDataStream::BigEndian);
    fullStream << quint32(header.size() + 4);  // 헤더 크기 + 크기 필드 자체
    fullHeader.append(header);

    return fullHeader;
}

bool FileTransfer::parseFileHeader(const QByteArray &data, QString &fileName, qint64 &fileSize)
{
    if (data.size() < 16) {  // 최소 크기
        return false;
    }

    QDataStream stream(data);
    stream.setByteOrder(QDataStream::BigEndian);

    quint32 headerSize;
    stream >> headerSize;

    if (data.size() < static_cast<int>(headerSize)) {
        return false;
    }

    quint32 magic;
    stream >> magic;

    if (magic != 0xF11E7562) {
        return false;
    }

    quint64 size;
    stream >> size;
    stream >> fileName;

    fileSize = size;

    // 파싱된 헤더 크기만큼 버퍼에서 제거
    receiveBuffer_.remove(0, headerSize);

    return true;
}
