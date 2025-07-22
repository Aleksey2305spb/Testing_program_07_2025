#ifndef TELECOMCLIENT_H
#define TELECOMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>

class TelecomClient : public QObject
{
    Q_OBJECT

public:
    explicit TelecomClient(QObject *parent = nullptr);

private slots:
    void on_Connected();  // вызывается при успешном подключении к серверу
    void read_Data();  // чтение данных от сервера
    void send_Network_Metrics();  // отправка данных сети
    void send_Device_Status();  // отправка статуса устройства
    void send_Log();  // отправка логов
    void connect_to_server();  // подключение и переподключение каждые 5 секунд
    void diconnect_server(); // отключение сервера
    void handle_Error(QAbstractSocket::SocketError error);  // обработка ошибок сокета
private:
    void start_Timers(); // запуск таймеров для периодической отправки данных
    void send_Json(const QJsonObject &json); // отправка сообщения

    QTcpSocket socket;
    qintptr clientId = -1;
};

#endif
