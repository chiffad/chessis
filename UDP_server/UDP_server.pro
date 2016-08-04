TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

SOURCES += src/main.cpp \
           src/udp_server.cpp \
           src/chess.cpp

HEADERS += headers/udp_server.h \
           headers/chess.h \
    headers/desk.h \
    headers/enums.h
