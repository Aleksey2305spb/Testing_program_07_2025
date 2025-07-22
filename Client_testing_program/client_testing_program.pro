QT += core network

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = TelecomClient
TEMPLATE = app

SOURCES += \
    main.cpp \
    telecomclient.cpp

HEADERS += \
    telecomclient.h


