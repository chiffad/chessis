#include <qqmlcontext.h>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QChar>
#include <QString>
#include "headers/integration.h"

QString figure(int index, QString color)
{
  QChar fig;
  if(index == 0 || index == 7) fig = 'r';
  if(index == 1 || index == 6) fig = 'h';
  if(index == 2 || index == 5) fig = 'e';
  if(index == 3) fig = 'q';
  if(index == 4) fig = 'k';

  if(color == "b_") return color + fig.toUpper();
  else return color + fig;
}

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  ChessIntegration model;
  for(int i = 0; i <= 7; ++i)
  {
    model.addFigure(ChessIntegration::Figure(figure(i, "w_"), i, 7, true));
    model.addFigure(ChessIntegration::Figure(figure(i, "b_"), i, 0, true));
    model.addFigure(ChessIntegration::Figure("w_p", i, 6, true));
    model.addFigure(ChessIntegration::Figure("b_P", i, 1, true));
  }

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("FigureModel", &model);
  engine.load(QUrl(QStringLiteral("chessis/res/app.qml")));

  return app.exec();
}
