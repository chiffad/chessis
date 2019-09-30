TEMPLATE = app

QT += qml quick \
      widgets \
      core\
      network widgets \
      opengl

CONFIG+=c++14

LIBS += -lboost_system\
        -lboost_serialization\
        #-L/home/dprokofiev/boost_1_63_0/stage/lib

HEADERS += cpp/board_graphic.h \
           cpp/messages.h \
           cpp/client.h \
           cpp/fb_obj.h \
           cpp/cube_renderer.h \
           cpp/coord.h \
           cpp/figure.h \
           cpp/helper.h \
           cpp/handle_message.h \

SOURCES += cpp/main.cpp\
           cpp/board_graphic.cpp \
           cpp/client.cpp \
           cpp/fb_obj.cpp \
           cpp/cube_renderer.cpp\
           cpp/helper.cpp \

RESOURCES += res.qrc

