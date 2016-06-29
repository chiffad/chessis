TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

SOURCES += src/main.cpp \
           src/udp_server.cpp \
           src/chess.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += headers/udp_server.h \
           headers/chess.h \
    headers/desk.h \
    headers/enums.h
