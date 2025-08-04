#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <QObject>
#include <QJsonObject>
#include "controllers/ClientSocket.h"

class UserService : public QObject
{
    Q_OBJECT

public:
    explicit UserService(ClientSocket* socket, QObject* parent = nullptr);

    void login(const QString& id, const QString& pw);
    void registerUser(const QString& id, const QString& pw,
                      const QString& name, const QString& email, const QString& address);
    void logout();

    ClientSocket* socket() const { return socket_; }

signals:
    void loginSuccess(const QJsonObject& userData);
    void loginFailed(const QString& reason);

    void registrationSuccess(const QJsonObject& userData);
    void registrationFailed(const QString& reason);

    void logoutSuccess();
    void logoutFailed();

private slots:
    void onLoginResponse(bool success, const QJsonObject& userData);
    void onRegistrationResponse(bool success, const QJsonObject& userData);
    void onLogoutResponse(bool success);

private:
    ClientSocket* socket_;
};

#endif // USERSERVICE_H
