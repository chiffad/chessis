QT += qml quick \
      widgets \
      core\
      network widgets

CONFIG+=c++14

HEADERS += headers/integration.h\
           headers/chess.h \
           headers/udp_client.h

SOURCES += src/integration.cpp \
           src/main.cpp\
           src/chess.cpp \
           src/udp_client.cpp

RESOURCES += \
    res.qrc

