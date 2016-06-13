#include <qqmlcontext.h>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlComponent>
#include <QQmlProperty>
#include "headers/board_graphic.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  Board_graphic model;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("FigureModel", &model);
  //engine.load(QUrl(QStringLiteral("chessis/res/app.qml")));

  return app.exec();
}
