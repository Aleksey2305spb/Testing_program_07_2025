#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QTcpSocket *socket, QObject *parent = nullptr);

signals:
    void data_Received(qintptr id, const QByteArray &data);
    void disconnected(qintptr id);

private slots:
    void on_Ready_Read();
    void on_Socket_Disconnected();

private:
    qintptr socket_Descriptor;
    QTcpSocket *socket;
};

#endif // CLIENTHANDLER_H
