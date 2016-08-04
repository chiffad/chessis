QT += qml quick \
      widgets \
      core\
      network widgets \
      opengl

CONFIG+=c++14

HEADERS +=\
    headers/board_graphic.h \
    headers/exporter.h \
    headers/enums.h \
    headers/udp_socket.h \
    headers/fb_obj.h \
    headers/cube_renderer.h

SOURCES += src/main.cpp\
           src/board_graphic.cpp \
           src/exporter.cpp \
           src/udp_socket.cpp \
           src/fb_obj.cpp \
           src/cube_renderer.cpp

RESOURCES += res.qrc

