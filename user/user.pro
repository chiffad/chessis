TEMPLATE = app

QT += qml quick \
      widgets \
      core\
      network widgets \
      opengl

CONFIG+=c++14

HEADERS += cpp/board_graphic.h \
           cpp/enums.h \
           cpp/client.h \
           cpp/fb_obj.h \
           cpp/cube_renderer.h \
           cpp/coord.h \
           cpp/figure.h \

SOURCES += cpp/main.cpp\
           cpp/board_graphic.cpp \
           cpp/client.cpp \
           cpp/fb_obj.cpp \
           cpp/cube_renderer.cpp

RESOURCES += res.qrc

