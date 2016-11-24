TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

SOURCES += cpp/main.cpp \
           cpp/udp_server.cpp \
           cpp/chess.cpp \
           cpp/desk.cpp \

HEADERS += cpp/udp_server.h \
           cpp/chess.h \
           cpp/desk.h \
           cpp/enums.h
