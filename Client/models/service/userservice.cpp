#include "UserService.h"
#include <QDebug>

UserService::UserService(ClientSocket* socket, QObject* parent)
    : QObject(parent), socket_(socket)
{
    connect(socket_, &ClientSocket::loginResponse, this, &UserService::onLoginResponse);
    connect(socket_, &ClientSocket::registrationResponse, this, &UserService::onRegistrationResponse);
    connect(socket_, &ClientSocket::logoutResponse, this, &UserService::onLogoutResponse);
}

void UserService::login(const QString& id, const QString& pw)
{
    socket_->login(id, pw);
}

void UserService::registerUser(const QString& id, const QString& pw,
                               const QString& name, const QString& email, const QString& address)
{
    socket_->registerUser(id, pw, name, email, address);
}

void UserService::logout()
{
    socket_->logout();
}

void UserService::onLoginResponse(bool success, const QJsonObject& userData)
{
    if (success) {
        emit loginSuccess(userData);
    } else {
        emit loginFailed("아이디나 비밀번호가 잘못되었습니다.");
    }
}

void UserService::onRegistrationResponse(bool success, const QJsonObject& userData)
{
    if (success) {
        emit registrationSuccess(userData);
    } else {
        emit registrationFailed("회원가입에 실패했습니다.");
    }
}

void UserService::onLogoutResponse(bool success)
{
    if (success) {
        emit logoutSuccess();
    } else {
        emit logoutFailed();
    }
}
