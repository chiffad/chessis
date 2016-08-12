#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QWindow>
#include <ctime>
#include "headers/board_graphic.h"
#include "headers/exporter.h"
#include "headers/udp_socket.h"
#include "headers/fb_obj.h"

bool is_gui_quit = false;
void gui_is_quit();

int main(int argc, char *argv[])
{
  qmlRegisterType<Fb_obj>("CubeRendering", 1, 0, "Cube");

  QGuiApplication *app = new QGuiApplication(argc, argv);
  QQmlApplicationEngine *engine = new QQmlApplicationEngine;
  Board_graphic *board_graphic = new Board_graphic;

  engine->rootContext()->setContextProperty("FigureModel", board_graphic);
  //engine->load(QUrl(QStringLiteral("../chessis/res/app.qml")));

  QObject::connect(engine, &QQmlApplicationEngine::quit, &gui_is_quit);

  UDP_socket *udp_socet = new UDP_socket;
  Exporter *exporter = new Exporter(board_graphic, udp_socet);

  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();
  while(!is_gui_quit)
  {
    app->processEvents();

    if((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while(board_graphic->is_new_message_for_server_appear())
        exporter->push_to_socet(exporter->pull_from_graphic());

      while(udp_socet->is_new_message_received())
        exporter->push_to_graphic(exporter->pull_from_socet());
    }

    //if(app->allWindows().isEmpty() || !app->allWindows().last()->visibility())
      //gui_is_quit();
  }

  delete exporter;
  delete udp_socet;
  delete board_graphic;
  delete engine;
  delete app;

  return 0;
}

void gui_is_quit()
{
  is_gui_quit = true;
}
