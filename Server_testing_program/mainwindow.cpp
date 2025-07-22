#include "mainwindow.h"
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    server = new Server(this); // инициализация  функционала сервера
    setup_UI(); // инициализация функционала сервера
    setup_connections(); // инициализация сигнал-слотов
}

void MainWindow::setup_UI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Кнопка
    start_server_button = new QPushButton("Start Server", this);

    // Таблица с клиентами
    clients_Table = new QTableWidget(0, 3, this);
    clients_Table->setHorizontalHeaderLabels({"ID", "IP Address", "Status"});
    clients_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Таблица данными
    data_Table = new QTableWidget(0, 4, this);
    data_Table->setHorizontalHeaderLabels({"Client ID", "Data Type", "Content", "Timestamp"});
    data_Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Таблица с логом данных
    log_Area = new QTextEdit(this);
    log_Area->setReadOnly(true);

    // Кнопка
    layout->addWidget(start_server_button);
    // Таблица с клиентами
    layout->addWidget(new QLabel("Connected Clients:"));
    layout->addWidget(clients_Table);
    // Таблица данными
    layout->addWidget(new QLabel("Received Data:"));
    layout->addWidget(data_Table);
    // Таблица с логом данных
    layout->addWidget(new QLabel("Server Log:"));
    layout->addWidget(log_Area);

    setCentralWidget(centralWidget);
    resize(1000, 800);
}

void MainWindow::setup_connections() {
    connect(start_server_button, &QPushButton::clicked, this, &MainWindow::on_Server); // включение сервера
    connect(server, &Server::log_Message, this, &MainWindow::log_Message); // обновление логов сообщений
    connect(server, &Server::client_List_Updated, this, &MainWindow::update_Client_List); // обновление списка клиентов
    connect(server, &Server::data_Received, this, &MainWindow::add_Data_Row); // обновление полученных данных
}

void MainWindow::on_Server() {
    if (server->isListening()) {
        server->stop_Server();
        start_server_button->setText("Start Server");
        log_Message("Server stopped");
    } else { // Включение сервера
        if (server->start_Server(12345)) {
            start_server_button->setText("Stop Server");
            log_Message("Server started on port 12345");
        }
    }
}

void MainWindow::update_Client_List() {
    clients_Table->setRowCount(0);
    auto clients = server->get_Clients();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        int row = clients_Table->rowCount();
        clients_Table->insertRow(row);
        clients_Table->setItem(row, 0, new QTableWidgetItem(QString::number(it.key())));
        clients_Table->setItem(row, 1, new QTableWidgetItem(it.value().ip));
        clients_Table->setItem(row, 2, new QTableWidgetItem(it.value().connected ? "Connected" : "Disconnected"));
    }
}

void MainWindow::log_Message(const QString &msg) {
    log_Area->append(QDateTime::currentDateTime().toString() + ": " + msg);
}

void MainWindow::add_Data_Row(qintptr id, const QString &type, const QString &content) {
    int row = data_Table->rowCount();
    data_Table->insertRow(row);
    data_Table->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    data_Table->setItem(row, 1, new QTableWidgetItem(type));
    data_Table->setItem(row, 2, new QTableWidgetItem(content));
    data_Table->setItem(row, 3, new QTableWidgetItem(QDateTime::currentDateTime().toString()));
}
