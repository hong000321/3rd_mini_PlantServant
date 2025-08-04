#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QDateTime>
#include "models/entity/user/User.h"
#include "UserManageService.h"
#include "utils/CustomErrorCodes.h"

struct SessionInfo {
    id_t userId;
    QString sessionId;
    QDateTime loginTime;
    QDateTime lastAccessTime;
    bool isActive;
};

class LoginService : public QObject
{
    Q_OBJECT

private:
    static LoginService* instance_;
    UserManageService* userService_;
    QMap<QString, SessionInfo> activeSessions_; // sessionId -> SessionInfo
    QMap<id_t, QString> userSessions_; // userId -> sessionId

    LoginService(QObject *parent = nullptr);
    LoginService(const LoginService&) = delete;
    LoginService& operator=(const LoginService&) = delete;

public:
    static LoginService* getInstance();
    static void destroyInstance();
    ~LoginService() = default;

    // 로그인/로그아웃 메서드들
    RaErrorCode login(const QString& strId, const QString& password, QString& outSessionId, User& outUser);
    RaErrorCode logout(const QString& sessionId);
    RaErrorCode logoutUser(id_t userId);

    // 세션 관리
    bool isSessionValid(const QString& sessionId);
    bool isUserLoggedIn(id_t userId);
    const SessionInfo* getSessionInfo(const QString& sessionId);
    id_t getUserBySession(const QString& sessionId);
    QString getSessionByUser(id_t userId);

    // 세션 갱신 및 정리
    void updateSessionAccess(const QString& sessionId);
    void cleanupExpiredSessions(int timeoutMinutes = 30);
    void forceLogoutUser(id_t userId);

    // 세션 정보
    QVector<SessionInfo> getActiveSessions();
    int getActiveSessionCount();
    QDateTime getLastLoginTime(id_t userId);

private:
    QString generateSessionId();
    void removeSession(const QString& sessionId);

signals:
    void userLoggedIn(id_t userId, const QString& sessionId);
    void userLoggedOut(id_t userId, const QString& sessionId);
    void sessionExpired(id_t userId, const QString& sessionId);
};

#endif // LOGINSERVICE_H
