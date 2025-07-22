#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QMap>
#include <QJsonObject>

struct ClientInfo {
    QString ip;
    bool connected = false;
    QJsonObject config;
};

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    bool start_Server(quint16 port);
    void stop_Server();
    QMap<qintptr, ClientInfo> get_Clients() const;
    bool is_Listening() const;

signals:
    void log_Message(const QString &msg);
    void client_List_Updated();
    void data_Received(qintptr clientId, const QString &type, const QString &content);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void on_Client_Data_Received(qintptr socketDescriptor, const QByteArray &data);
    void on_Client_Disconnected(qintptr socketDescriptor);

private:
    void send_Connection_Ack(QTcpSocket *socket, const QString &ip);
    void process_Client_Data(qintptr id, const QByteArray &data);
    void update_Client_Status(qintptr id, bool connected);

    QMap<qintptr, ClientInfo> clientInfo;
    QMap<qintptr, QTcpSocket*> clientSockets;
};

#endif // SERVER_H
