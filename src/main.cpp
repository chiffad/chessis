#include "headers/integration.h"
#include <qqmlcontext.h>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlComponent>
#include <QQmlProperty>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  ChessIntegration model;
  model.addFigure(Figure("w_r", 0, 7, true));
  model.addFigure(Figure("w_h", 1, 7, true));
  model.addFigure(Figure("w_e", 2, 7, true));
  model.addFigure(Figure("w_q", 3, 7, true));
  model.addFigure(Figure("w_k", 4, 7, true));
  model.addFigure(Figure("w_e", 5, 7, true));
  model.addFigure(Figure("w_h", 6, 7, true));
  model.addFigure(Figure("w_r", 7, 7, true));
  model.addFigure(Figure("w_p", 0, 6, true));
  model.addFigure(Figure("w_p", 1, 6, true));
  model.addFigure(Figure("w_p", 2, 6, true));
  model.addFigure(Figure("w_p", 3, 6, true));
  model.addFigure(Figure("w_p", 4, 6, true));
  model.addFigure(Figure("w_p", 5, 6, true));
  model.addFigure(Figure("w_p", 6, 6, true));
  model.addFigure(Figure("w_p", 7, 6, true));

  model.addFigure(Figure("b_R", 0, 0, true));
  model.addFigure(Figure("b_H", 1, 0, true));
  model.addFigure(Figure("b_E", 2, 0, true));
  model.addFigure(Figure("b_Q", 3, 0, true));
  model.addFigure(Figure("b_K", 4, 0, true));
  model.addFigure(Figure("b_E", 5, 0, true));
  model.addFigure(Figure("b_H", 6, 0, true));
  model.addFigure(Figure("b_R", 7, 0, true));
  model.addFigure(Figure("b_P", 0, 1, true));
  model.addFigure(Figure("b_P", 1, 1, true));
  model.addFigure(Figure("b_P", 2, 1, true));
  model.addFigure(Figure("b_P", 3, 1, true));
  model.addFigure(Figure("b_P", 4, 1, true));
  model.addFigure(Figure("b_P", 5, 1, true));
  model.addFigure(Figure("b_P", 6, 1, true));
  model.addFigure(Figure("b_P", 7, 1, true));

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("FigureModel", &model);
  engine.load(QUrl(QStringLiteral("chessis/res/app.qml")));

  return app.exec();
}
