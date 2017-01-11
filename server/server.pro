TEMPLATE = app

QT += qml quick\
      network widgets

CONFIG += c++14

LIBS += -lboost_system\
        -lboost_serialization\
        -L/home/dprokofiev/boost_1_63_0/stage/lib

SOURCES += cpp/main.cpp \
           cpp/chess.cpp \
           cpp/handle_message.cpp \
           cpp/desk.cpp \
           cpp/messages.cpp \
           cpp/server.cpp \
           cpp/client.cpp \


HEADERS += cpp/chess.h \
           cpp/handle_message.h \
           cpp/desk.h \
           cpp/messages.h \
           cpp/server.h \
           cpp/coord.h \
           cpp/client.h \
           cpp/helper.h \

