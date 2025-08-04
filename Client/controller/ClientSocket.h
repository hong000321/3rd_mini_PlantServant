#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = nullptr);

signals:
};

#endif // CLIENTSOCKET_H
