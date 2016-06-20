QT += qml quick \
      widgets \
      core\
      network widgets

CONFIG+=c++14

HEADERS +=\
    headers/board_graphic.h \
    headers/udp_socet.h \
    headers/exporter.h

SOURCES += \
           src/main.cpp\
    src/board_graphic.cpp \
    src/udp_socet.cpp \
    src/exporter.cpp

RESOURCES += \
    res.qrc

