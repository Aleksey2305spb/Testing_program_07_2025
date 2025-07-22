QT += core gui network widgets

CONFIG += c++17

TARGET = TelecomServer
TEMPLATE = app

SOURCES += \
    clienthandler.cpp \
    main.cpp \
    mainwindow.cpp \
    server.cpp

HEADERS += \
    clienthandler.h \
    mainwindow.h \
    server.h

FORMS += \
    mainwindow.ui
