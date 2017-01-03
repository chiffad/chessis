TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

LIBS += -lboost_system\   # for boost

SOURCES += cpp/main.cpp \
           cpp/chess.cpp \
           cpp/desk.cpp \
           cpp/messages.cpp \
           cpp/server.cpp \
           cpp/client.cpp \


HEADERS += cpp/chess.h \
           cpp/desk.h \
           cpp/messages.h \
           cpp/server.h \
           cpp/coord.h \
           cpp/client.h \
           cpp/helper.h \

