#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

class QTableWidget;
class QTextEdit;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void on_Server();
    void update_Client_List();
    void log_Message(const QString &msg);
    void add_Data_Row(qintptr id, const QString &type, const QString &content);

private:
    void setup_UI();
    void setup_connections();

    QPushButton *start_server_button;
    QTableWidget *clients_Table;
    QTableWidget *data_Table;
    QTextEdit *log_Area;
    Server *server;
};

#endif // MAINWINDOW_H
