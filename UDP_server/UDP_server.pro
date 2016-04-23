TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

SOURCES += main.cpp \
    udp_server.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    udp_server.h
