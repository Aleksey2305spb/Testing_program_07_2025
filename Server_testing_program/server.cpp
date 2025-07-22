#include "server.h"
#include "clienthandler.h"
#include <QJsonDocument>
#include <QHostAddress>
#include <QTcpSocket>

Server::Server(QObject *parent) : QTcpServer(parent) {}

bool Server::start_Server(quint16 port) {
    if (listen(QHostAddress::Any, port)) {
        emit log_Message("Listening on port " + QString::number(port));
        return true;
    }
    emit log_Message("Error: " + errorString());
    return false;
}

void Server::stop_Server() {
    for (auto socket : clientSockets) {
        socket->disconnectFromHost();
        if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->waitForDisconnected(1000);
        }
        socket->deleteLater();
    }
    clientSockets.clear();
    clientInfo.clear();
    close();
    emit log_Message("Server stopped");
}

bool Server::is_Listening() const {
    return QTcpServer::isListening();
}

QMap<qintptr, ClientInfo> Server::get_Clients() const {
    return clientInfo;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit log_Message("Socket error: " + socket->errorString());
        delete socket;
        return;
    }

    QString ip = socket->peerAddress().toString();
    clientInfo[socketDescriptor] = {ip, true, QJsonObject()};
    clientSockets[socketDescriptor] = socket;

    ClientHandler *handler = new ClientHandler(socketDescriptor, socket, this);
    connect(handler, &ClientHandler::data_Received, this, &Server::on_Client_Data_Received);
    connect(handler, &ClientHandler::disconnected, this, &Server::on_Client_Disconnected);

    send_Connection_Ack(socket, ip);
    emit client_List_Updated();
    emit log_Message("New connection: " + ip);
}

void Server::on_Client_Data_Received(qintptr socketDescriptor, const QByteArray &data) {
    process_Client_Data(socketDescriptor, data);
}

void Server::on_Client_Disconnected(qintptr socketDescriptor) {
    if (clientSockets.contains(socketDescriptor)) {
        clientSockets[socketDescriptor]->deleteLater();
        clientSockets.remove(socketDescriptor);
    }
    update_Client_Status(socketDescriptor, false);
    clientInfo.remove(socketDescriptor);
    emit client_List_Updated();
    emit log_Message("Client disconnected: " + QString::number(socketDescriptor));
}

void Server::send_Connection_Ack(QTcpSocket *socket, const QString &ip) {
    QJsonObject ack;
    ack["status"] = "connected";
    ack["client_id"] = (qint64)socket->socketDescriptor();
    ack["ip"] = ip;
    socket->write(QJsonDocument(ack).toJson());
    socket->flush();
}

void Server::process_Client_Data(qintptr id, const QByteArray &data) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        emit log_Message("JSON error: " + error.errorString());
        return;
    }

    QJsonObject json = doc.object();
    QString type = json["type"].toString();
    QString content = QJsonDocument(json).toJson(QJsonDocument::Compact);

    if (type == "NetworkMetrics" || type == "DeviceStatus" || type == "Log") {
        emit data_Received(id, type, content);
    }

    if (type == "DeviceStatus") {
        int cpuUsage = json["cpu_usage"].toInt();
        int threshold = clientInfo[id].config.contains("cpu_threshold")
                       ? clientInfo[id].config["cpu_threshold"].toInt()
                       : 90;
        if (cpuUsage > threshold) {
            emit log_Message("WARNING: High CPU usage on client " + QString::number(id));
        }
    }
}

void Server::update_Client_Status(qintptr id, bool connected) {
    if (clientInfo.contains(id)) {
        clientInfo[id].connected = connected;
    }
}
