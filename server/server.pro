TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

SOURCES += cpp/main.cpp \
           cpp/udp_server.cpp \
           cpp/chess.cpp \
           cpp/desk.cpp \
           cpp/server.cpp \
           cpp/server_user.cpp \
           cpp/user.cpp \


HEADERS += cpp/udp_server.h \
           cpp/chess.h \
           cpp/desk.h \
           cpp/messages.h \
           cpp/server.h \
           cpp/coord.h \
           cpp/server_user.h \
           cpp/user.h \
