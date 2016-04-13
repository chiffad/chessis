QT += qml quick \
      widgets \
      core

CONFIG+=c++14

HEADERS += headers/integration.h\
           headers/chess.h
SOURCES += src/integration.cpp \
           src/main.cpp\
           src/chess.cpp

RESOURCES += \
    res.qrc

