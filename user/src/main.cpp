#include "client/client_controller.hpp"
#include "controller/app.hpp"
#include "logger.hpp"
#include "message_handler/handler.hpp"

#include <messages/messages.hpp>
#include <spdlog/spdlog.h>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QTimer>

int main(int argc, char* argv[])
{
  chess::logger::logger_t::get().init();

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  qmlRegisterSingletonType(QUrl("qrc:/res/Constants.qml"), "Constants", 1, 0, "Constants");

  QGuiApplication app(argc, argv);
  const auto invoke_on_main_thread = [&app](std::function<void()> fn) { QTimer::singleShot(0, /*app,*/ std::move(fn)); };

  std::unique_ptr<chess::controller::app_t> app_controller;
  std::unique_ptr<chess::message_handler::handler_t> msg_handler;

  const chess::cl::client_t::message_received_callback_t message_received_callback = [&](chess::msg::some_datagram_t d) {
    invoke_on_main_thread([&, d = std::move(d)] { msg_handler->handle(std::move(d)); });
  };
  const chess::cl::client_t::server_status_changed_callback_t server_status_changed = [&](bool server_online) {
    invoke_on_main_thread([&, server_online] { app_controller->server_status_changed(server_online); });
  };
  chess::cl::client_controller_t client{message_received_callback, server_status_changed};

  chess::controller::app_t::command_requested_callbacks_t controller_callbacks;
  controller_callbacks.login = [&](const std::string& login, const std::string& pwd) { invoke_on_main_thread([&, login, pwd]() { client.send_login(login, pwd); }); };
  controller_callbacks.move = [&](std::string data) { invoke_on_main_thread([&, data = std::move(data)]() { client.send_move(std::move(data)); }); };
  controller_callbacks.go_to_history = [&](uint16_t history_i) { invoke_on_main_thread([&, history_i]() { client.send_go_to_history(history_i); }); };
  controller_callbacks.new_game = [&]() { invoke_on_main_thread([&]() { client.send_new_game(); }); };
  controller_callbacks.back_move = [&]() { invoke_on_main_thread([&]() { client.send_back_move(); }); };
  controller_callbacks.opponent_inf = [&]() { invoke_on_main_thread([&]() { client.send_opponent_inf(); }); };
  controller_callbacks.my_inf = [&]() { invoke_on_main_thread([&]() { client.send_my_inf(); }); };

  app_controller = std::make_unique<chess::controller::app_t>(controller_callbacks);
  msg_handler = std::make_unique<chess::message_handler::handler_t>(*app_controller, client);

  return app.exec();
}
