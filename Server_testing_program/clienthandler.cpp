#include "clienthandler.h"
#include <QDebug>

ClientHandler::ClientHandler(qintptr socket_Descriptor, QTcpSocket *socket, QObject *parent)
    : QObject(parent), socket_Descriptor(socket_Descriptor), socket(socket)
{
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::on_Ready_Read);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::on_Socket_Disconnected);
}

void ClientHandler::on_Ready_Read() {
    QByteArray data = socket->readAll();
    if (!data.isEmpty()) {
        emit data_Received(socket_Descriptor, data);
    }
}

void ClientHandler::on_Socket_Disconnected() {
    emit disconnected(socket_Descriptor);
    socket->deleteLater();
    deleteLater();
}
