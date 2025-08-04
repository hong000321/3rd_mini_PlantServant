#include "LoginService.h"
#include <QUuid>
#include <QDebug>

LoginService* LoginService::instance_ = nullptr;

LoginService::LoginService(QObject *parent)
    : QObject(parent)
{
    userService_ = UserManageService::getInstance();
}

LoginService* LoginService::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new LoginService();
    }
    return instance_;
}

void LoginService::destroyInstance()
{
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

RaErrorCode LoginService::login(const QString& strId, const QString& password, QString& outSessionId, User& outUser)
{
    // 사용자 인증
    RaErrorCode authResult = userService_->authenticateUser(strId, password, outUser);
    if (authResult != Ra_Success) {
        return authResult;
    }

    // 이미 로그인된 사용자인지 확인
    if (isUserLoggedIn(outUser.getId())) {
        // 기존 세션 종료
        logoutUser(outUser.getId());
    }

    // 새 세션 생성
    outSessionId = generateSessionId();
    SessionInfo sessionInfo;
    sessionInfo.userId = outUser.getId();
    sessionInfo.sessionId = outSessionId;
    sessionInfo.loginTime = QDateTime::currentDateTime();
    sessionInfo.lastAccessTime = QDateTime::currentDateTime();
    sessionInfo.isActive = true;

    activeSessions_[outSessionId] = sessionInfo;
    userSessions_[outUser.getId()] = outSessionId;

    // 사용자 연결 상태 업데이트
    userService_->connectUser(outUser.getId());

    emit userLoggedIn(outUser.getId(), outSessionId);
    qDebug() << "User logged in:" << strId << "Session:" << outSessionId;

    return Ra_Success;
}

RaErrorCode LoginService::logout(const QString& sessionId)
{
    if (!activeSessions_.contains(sessionId)) {
        return Ra_Domain_Unkown_Error;
    }

    SessionInfo sessionInfo = activeSessions_[sessionId];
    id_t userId = sessionInfo.userId;

    // 사용자 연결 상태 업데이트
    userService_->disconnectUser(userId);

    removeSession(sessionId);

    emit userLoggedOut(userId, sessionId);
    qDebug() << "User logged out, Session:" << sessionId;

    return Ra_Success;
}

RaErrorCode LoginService::logoutUser(id_t userId)
{
    if (!userSessions_.contains(userId)) {
        return Ra_Domain_Unkown_Error;
    }

    QString sessionId = userSessions_[userId];
    return logout(sessionId);
}

bool LoginService::isSessionValid(const QString& sessionId)
{
    return activeSessions_.contains(sessionId) && activeSessions_[sessionId].isActive;
}

bool LoginService::isUserLoggedIn(id_t userId)
{
    return userSessions_.contains(userId) && isSessionValid(userSessions_[userId]);
}

const SessionInfo* LoginService::getSessionInfo(const QString& sessionId)
{
    if (activeSessions_.contains(sessionId)) {
        return &activeSessions_[sessionId];
    }
    return nullptr;
}

id_t LoginService::getUserBySession(const QString& sessionId)
{
    if (activeSessions_.contains(sessionId)) {
        return activeSessions_[sessionId].userId;
    }
    return -1;
}

QString LoginService::getSessionByUser(id_t userId)
{
    return userSessions_.value(userId, "");
}

void LoginService::updateSessionAccess(const QString& sessionId)
{
    if (activeSessions_.contains(sessionId)) {
        activeSessions_[sessionId].lastAccessTime = QDateTime::currentDateTime();
    }
}

void LoginService::cleanupExpiredSessions(int timeoutMinutes)
{
    QDateTime cutoffTime = QDateTime::currentDateTime().addSecs(-timeoutMinutes * 60);
    QStringList expiredSessions;

    for (auto it = activeSessions_.begin(); it != activeSessions_.end(); ++it) {
        if (it.value().lastAccessTime < cutoffTime) {
            expiredSessions.append(it.key());
        }
    }

    for (const QString& sessionId : expiredSessions) {
        SessionInfo sessionInfo = activeSessions_[sessionId];
        emit sessionExpired(sessionInfo.userId, sessionId);
        removeSession(sessionId);
    }
}

void LoginService::forceLogoutUser(id_t userId)
{
    if (userSessions_.contains(userId)) {
        QString sessionId = userSessions_[userId];
        userService_->disconnectUser(userId);
        removeSession(sessionId);
        emit userLoggedOut(userId, sessionId);
    }
}

QVector<SessionInfo> LoginService::getActiveSessions()
{
    QVector<SessionInfo> sessions;
    for (auto it = activeSessions_.begin(); it != activeSessions_.end(); ++it) {
        if (it.value().isActive) {
            sessions.append(it.value());
        }
    }
    return sessions;
}

int LoginService::getActiveSessionCount()
{
    int count = 0;
    for (auto it = activeSessions_.begin(); it != activeSessions_.end(); ++it) {
        if (it.value().isActive) {
            count++;
        }
    }
    return count;
}

QDateTime LoginService::getLastLoginTime(id_t userId)
{
    if (userSessions_.contains(userId)) {
        QString sessionId = userSessions_[userId];
        if (activeSessions_.contains(sessionId)) {
            return activeSessions_[sessionId].loginTime;
        }
    }
    return QDateTime();
}

QString LoginService::generateSessionId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

void LoginService::removeSession(const QString& sessionId)
{
    if (activeSessions_.contains(sessionId)) {
        id_t userId = activeSessions_[sessionId].userId;
        activeSessions_.remove(sessionId);
        userSessions_.remove(userId);
    }
}
