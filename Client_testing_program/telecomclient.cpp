#include "telecomclient.h"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QDateTime>

TelecomClient::TelecomClient(QObject *parent) : QObject(parent)
{
    connect(&socket, &QTcpSocket::connected, this, &TelecomClient::on_Connected);
    connect(&socket, &QTcpSocket::readyRead, this, &TelecomClient::read_Data);
    connect(&socket, &QTcpSocket::disconnected, this, &TelecomClient::diconnect_server);
    connect(&socket, &QTcpSocket::errorOccurred, this, &TelecomClient::handle_Error);

    start_Timers();
    connect_to_server(); // подключение и переподключение каждые 5 секунд
}

void TelecomClient::on_Connected()
{
    qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]")
             << "Connected to server";
}

void TelecomClient::read_Data()
{
    QByteArray data = socket.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Received invalid JSON";
        return;
    }

    QJsonObject json = doc.object();
    if (json["status"] == "connected") {
        clientId = json["client_id"].toInt();
        qDebug() << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]")
                 << " ID:" << clientId;
    }
}

void TelecomClient::send_Network_Metrics()
{
    QJsonObject metrics;
    metrics["type"] = "NetworkMetrics";

    float bandwidth = 10.0f + QRandomGenerator::global()->generateDouble() * 990.0f;
    float latency = 1.0f + QRandomGenerator::global()->generateDouble() * 99.0f;
    float packet_loss = QRandomGenerator::global()->generateDouble() * 5.0f;

    metrics["bandwidth"] = bandwidth;
    metrics["latency"] = latency;
    metrics["packet_loss"] = packet_loss;

    send_Json(metrics);
}

void TelecomClient::send_Device_Status()
{
    QJsonObject status;
    status["type"] = "DeviceStatus";
    status["uptime"] = QRandomGenerator::global()->bounded(3600, 86400);
    status["cpu_usage"] = QRandomGenerator::global()->bounded(0, 100);
    status["memory_usage"] = QRandomGenerator::global()->bounded(0, 100);

    send_Json(status);
}

void TelecomClient::send_Log()
{
    static const QStringList severities = {"INFO", "WARNING", "ERROR"};
    static const QStringList messages = {
        "System initialized",
        "Temperature within normal range",
        "Packet loss detected",
        "CPU usage exceeded threshold",
        "Memory allocation failed",
        "Network interface reset"
    };

    QJsonObject log;
    log["type"] = "Log";
    log["severity"] = severities.at(QRandomGenerator::global()->bounded(severities.size()));

    QString msg = messages.at(QRandomGenerator::global()->bounded(messages.size()));
    int repeat = 1 + QRandomGenerator::global()->bounded(9);
    log["message"] = msg.repeated(repeat);

    send_Json(log);
}

void TelecomClient::connect_to_server() // подключение и переподключение каждые 5 секунд
{
    if (!socket.isOpen()) {
        qDebug() << "Connecting to server...";
        socket.connectToHost("localhost", 12345);
        QTimer::singleShot(5000, this, &TelecomClient::connect_to_server);
    }
}
void TelecomClient::diconnect_server ()
{
    qDebug() << "Connecting failed ...";
}


void TelecomClient::handle_Error(QAbstractSocket::SocketError error)
{
    qDebug() << "Socket error: " << socket.errorString();
    socket.close();
    QTimer::singleShot(5000, this, &TelecomClient::connect_to_server);
}


void TelecomClient::start_Timers()
{

    QTimer *metricsTimer = new QTimer(this);
    connect(metricsTimer, &QTimer::timeout, this, &TelecomClient::send_Network_Metrics);
    metricsTimer->start(1000 / (1 + QRandomGenerator::global()->bounded(10)));

    QTimer *statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &TelecomClient::send_Device_Status);
    statusTimer->start(100 + QRandomGenerator::global()->bounded(400));

    QTimer *logTimer = new QTimer(this);
    connect(logTimer, &QTimer::timeout, this, &TelecomClient::send_Log);
    logTimer->start(500 + QRandomGenerator::global()->bounded(1500));
}


void TelecomClient::send_Json(const QJsonObject &json)
{
    if (socket.state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(json);
        QByteArray jsonData = doc.toJson();

        // Дублирование вывода в консоль
        qDebug() << jsonData;

        socket.write(jsonData);
    }
}
