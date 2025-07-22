#include "telecomclient.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qSetMessagePattern("%{time [yyyy-MM-dd hh:mm:ss]} %{message}"); // установка формата вывода логов

    TelecomClient client;
    return app.exec();
}
