#include "headers/integration.h"
#include "headers/chess.h"
#include <QtQuick/QQuickView>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  qmlRegisterType<Integr>("Integration", 1, 0, "IntegrationClass");

  QQuickView view;
  view.setResizeMode(QQuickView::SizeRootObjectToView);
  view.setSource(QUrl("qrc:///res/app.qml"));
  view.show();
  return app.exec();
}

