QT += qml quick \
      widgets \
      core\
      network widgets

CONFIG+=c++14

HEADERS +=\
    headers/board_graphic.h \
    headers/exporter.h \
    headers/enums.h \
    headers/udp_socket.h

SOURCES += \
           src/main.cpp\
    src/board_graphic.cpp \
    src/exporter.cpp \
    src/udp_socket.cpp

RESOURCES += \
    res.qrc

