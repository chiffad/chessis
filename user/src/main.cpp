#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QWindow>
#include <ctime>
#include <deque>
#include <exception>

#include <spdlog/spdlog.h>

#include "client/client.h"
#include "controller/app.hpp"
#include "logger.hpp"
#include "message_handler/handler.hpp"

struct messages_deque_t : std::deque<std::string>
{
  template<typename MSG_T, typename... Args>
  void add(Args&&... args)
  {
    push_back(msg::prepare_for_send(MSG_T{std::forward<Args>(args)...}));
  }
};

int main(int argc, char* argv[])
{
  logger::logger_t::get().init();

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  qmlRegisterSingletonType(QUrl("qrc:/res/Constants.qml"), "Constants", 1, 0, "Constants");

  messages_deque_t received_messages;
  messages_deque_t messages_to_send;

  QGuiApplication app(argc, argv);

  controller::app_t::command_requested_callbacks_t controller_callbacks;
  // TODO: add token to messages!
  /*controller_callbacks.login = [&messages_to_send](const std::string& login, const std::string& pwd) { messages_to_send.add<msg::login_t>(login, pwd); };
  controller_callbacks.move = [&messages_to_send](std::string data) { messages_to_send.add<msg::move_t>(std::move(data)); };
  controller_callbacks.go_to_history = [&messages_to_send](size_t history_i) { messages_to_send.add<msg::go_to_history_t>(history_i); };
  controller_callbacks.new_game = [&messages_to_send]() { messages_to_send.add<msg::new_game_t>(); };
  controller_callbacks.back_move = [&messages_to_send]() { messages_to_send.add<msg::back_move_t>(); };
  controller_callbacks.opponent_inf = [&messages_to_send]() { messages_to_send.add<msg::opponent_inf_t>(); };
  controller_callbacks.my_inf = [&messages_to_send]() { messages_to_send.add<msg::my_inf_t>(); };*/

  controller::app_t app_controller(controller_callbacks);

  const cl::client_t::message_received_callback_t message_received_callback = [&](std::string str) { received_messages.push_back(std::move(str)); };
  const cl::client_t::server_status_changed_callback_t server_status_changed = [&](bool server_online) { app_controller.server_status_changed(server_online); };
  std::unique_ptr<cl::connection_strategy_t> connection_strategy =
    std::make_unique<cl::search_port_t>(cl::endpoint_t{"127.0.0.1", cl::login_server::FIRST_PORT}, cl::login_server::FIRST_PORT, cl::login_server::LAST_PORT);
  cl::client_t client(message_received_callback, server_status_changed, std::move(connection_strategy));

  message_handler::handler_t msg_handler{app_controller};

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
        msg_handler.handle(received_messages.front());
        received_messages.pop_front();
      }
    }
  }

  return 0;
}
