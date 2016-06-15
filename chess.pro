QT += qml quick \
      widgets \
      core\
      network widgets

CONFIG+=c++14

HEADERS +=\
           headers/udp_client.h \
    headers/board_graphic.h \
    headers/udp_socet.h

SOURCES += \
           src/main.cpp\
           src/udp_client.cpp \
    src/board_graphic.cpp \
    src/udp_socet.cpp

RESOURCES += \
    res.qrc

