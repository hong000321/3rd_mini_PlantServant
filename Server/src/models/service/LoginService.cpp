#include "LoginService.h"
#include <QUuid>
#include <QDebug>

LoginService* LoginService::instance = nullptr;

LoginService::LoginService(QObject *parent)
    : QObject(parent)
{
    userService = UserManageService::getInstance();
}

LoginService* LoginService::getInstance()
{
    if (instance == nullptr) {
        instance = new LoginService();
    }
    return instance;
}

void LoginService::destroyInstance()
{
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

RaErrorCode LoginService::login(const QString& strId, const QString& password, QString& outSessionId, User& outUser)
{
    // 사용자 인증
    RaErrorCode authResult = userService->authenticateUser(strId, password, outUser);
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

    activeSessions[outSessionId] = sessionInfo;
    userSessions[outUser.getId()] = outSessionId;

    // 사용자 연결 상태 업데이트
    userService->connectUser(outUser.getId());

    emit userLoggedIn(outUser.getId(), outSessionId);
    qDebug() << "User logged in:" << strId << "Session:" << outSessionId;

    return Ra_Success;
}

RaErrorCode LoginService::logout(const QString& sessionId)
{
    if (!activeSessions.contains(sessionId)) {
        return Ra_Domain_Unkown_Error;
    }

    SessionInfo sessionInfo = activeSessions[sessionId];
    id_t userId = sessionInfo.userId;

    // 사용자 연결 상태 업데이트
    userService->disconnectUser(userId);

    removeSession(sessionId);

    emit userLoggedOut(userId, sessionId);
    qDebug() << "User logged out, Session:" << sessionId;

    return Ra_Success;
}

RaErrorCode LoginService::logoutUser(id_t userId)
{
    if (!userSessions.contains(userId)) {
        return Ra_Domain_Unkown_Error;
    }

    QString sessionId = userSessions[userId];
    return logout(sessionId);
}

bool LoginService::isSessionValid(const QString& sessionId)
{
    return activeSessions.contains(sessionId) && activeSessions[sessionId].isActive;
}

bool LoginService::isUserLoggedIn(id_t userId)
{
    return userSessions.contains(userId) && isSessionValid(userSessions[userId]);
}

const SessionInfo* LoginService::getSessionInfo(const QString& sessionId)
{
    if (activeSessions.contains(sessionId)) {
        return &activeSessions[sessionId];
    }
    return nullptr;
}

id_t LoginService::getUserBySession(const QString& sessionId)
{
    if (activeSessions.contains(sessionId)) {
        return activeSessions[sessionId].userId;
    }
    return -1;
}

QString LoginService::getSessionByUser(id_t userId)
{
    return userSessions.value(userId, "");
}

void LoginService::updateSessionAccess(const QString& sessionId)
{
    if (activeSessions.contains(sessionId)) {
        activeSessions[sessionId].lastAccessTime = QDateTime::currentDateTime();
    }
}

void LoginService::cleanupExpiredSessions(int timeoutMinutes)
{
    QDateTime cutoffTime = QDateTime::currentDateTime().addSecs(-timeoutMinutes * 60);
    QStringList expiredSessions;

    for (auto it = activeSessions.begin(); it != activeSessions.end(); ++it) {
        if (it.value().lastAccessTime < cutoffTime) {
            expiredSessions.append(it.key());
        }
    }

    for (const QString& sessionId : expiredSessions) {
        SessionInfo sessionInfo = activeSessions[sessionId];
        emit sessionExpired(sessionInfo.userId, sessionId);
        removeSession(sessionId);
    }
}

void LoginService::forceLogoutUser(id_t userId)
{
    if (userSessions.contains(userId)) {
        QString sessionId = userSessions[userId];
        userService->disconnectUser(userId);
        removeSession(sessionId);
        emit userLoggedOut(userId, sessionId);
    }
}

QVector<SessionInfo> LoginService::getActiveSessions()
{
    QVector<SessionInfo> sessions;
    for (auto it = activeSessions.begin(); it != activeSessions.end(); ++it) {
        if (it.value().isActive) {
            sessions.append(it.value());
        }
    }
    return sessions;
}

int LoginService::getActiveSessionCount()
{
    int count = 0;
    for (auto it = activeSessions.begin(); it != activeSessions.end(); ++it) {
        if (it.value().isActive) {
            count++;
        }
    }
    return count;
}

QDateTime LoginService::getLastLoginTime(id_t userId)
{
    if (userSessions.contains(userId)) {
        QString sessionId = userSessions[userId];
        if (activeSessions.contains(sessionId)) {
            return activeSessions[sessionId].loginTime;
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
    if (activeSessions.contains(sessionId)) {
        id_t userId = activeSessions[sessionId].userId;
        activeSessions.remove(sessionId);
        userSessions.remove(userId);
    }
}
