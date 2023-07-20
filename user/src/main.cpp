#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QWindow>
#include <ctime>
#include <deque>
#include <exception>

#include "logger.hpp"
#include <spdlog/spdlog.h>

#include "client/client.h"
#include "controller/app.hpp"

using messages_deque_t = std::deque<std::string>;

int main(int argc, char* argv[])
{
  logger::logger_t::get().init();

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  qmlRegisterSingletonType(QUrl("qrc:/res/Constants.qml"), "Constants", 1, 0, "Constants");

  messages_deque_t received_messages;
  messages_deque_t messages_to_send;

  QGuiApplication app(argc, argv);
  controller::app_t app_controller([&](std::string str) { messages_to_send.push_back(std::move(str)); });
  cl::client_t client{[&](std::string str) { received_messages.push_back(std::move(str)); }};

  const double CHECK_TIME = 0.015 * CLOCKS_PER_SEC;
  clock_t timer = clock();
  while (!app.allWindows().isEmpty() && app.allWindows().last()->visibility())
  {
    app.processEvents();

    if ((clock() - timer) > CHECK_TIME)
    {
      timer = clock();

      while (!messages_to_send.empty())
      {
        client.send(messages_to_send.front());
        messages_to_send.pop_front();
      }

      while (!received_messages.empty())
      {
        app_controller.process(received_messages.front());
        received_messages.pop_front();
      }
    }
  }

  return 0;
}
