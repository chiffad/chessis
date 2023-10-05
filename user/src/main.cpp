#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QWindow>
#include <ctime>
#include <deque>
#include <exception>

#include <spdlog/spdlog.h>

#include "client/client_controller.hpp"
#include "controller/app.hpp"
#include "logger.hpp"
#include "message_handler/handler.hpp"

using messages_deque_t = std::deque<std::string>;

int main(int argc, char* argv[])
{
  logger::logger_t::get().init();

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  qmlRegisterSingletonType(QUrl("qrc:/res/Constants.qml"), "Constants", 1, 0, "Constants");

  QGuiApplication app(argc, argv);
  std::unique_ptr<controller::app_t> app_controller;

  messages_deque_t received_messages;
  // TODO: move to client_controller_t
  messages_deque_t messages_to_send;

  const cl::client_t::message_received_callback_t message_received_callback = [&](std::string str) { received_messages.push_back(std::move(str)); };
  const cl::client_t::server_status_changed_callback_t server_status_changed = [&](bool server_online) { app_controller->server_status_changed(server_online); };
  cl::client_controller_t client{message_received_callback, server_status_changed};

  controller::app_t::command_requested_callbacks_t controller_callbacks;
  controller_callbacks.login = [&](const std::string& login, const std::string& pwd) { messages_to_send.push_back(client.prepare_for_send<msg::login_t>(login, pwd)); };
  controller_callbacks.move = [&](std::string data) { messages_to_send.push_back(client.prepare_for_send<msg::move_t>(std::move(data))); };
  controller_callbacks.go_to_history = [&](size_t history_i) { messages_to_send.push_back(client.prepare_for_send<msg::go_to_history_t>(history_i)); };
  controller_callbacks.new_game = [&]() { messages_to_send.push_back(client.prepare_for_send<msg::new_game_t>()); };
  controller_callbacks.back_move = [&]() { messages_to_send.push_back(client.prepare_for_send<msg::back_move_t>()); };
  controller_callbacks.opponent_inf = [&]() { messages_to_send.push_back(client.prepare_for_send<msg::opponent_inf_t>()); };
  controller_callbacks.my_inf = [&]() { messages_to_send.push_back(client.prepare_for_send<msg::my_inf_t>()); };

  app_controller = std::make_unique<controller::app_t>(controller_callbacks);
  message_handler::handler_t msg_handler{*app_controller};

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
        client.send(std::move(messages_to_send.front()));
        messages_to_send.pop_front();
      }

      while (!received_messages.empty())
      {
        msg_handler.handle(std::move(received_messages.front()));
        received_messages.pop_front();
      }
    }
  }

  return 0;
}
