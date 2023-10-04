#pragma once

#include "controller/board.hpp"
#include "controller/login_input.hpp"
#include "controller/menu_layout.hpp"
#include "controller/message_processor.hpp"

#include <QQmlApplicationEngine>

namespace controller {

class app_t
{
public:
  struct command_requested_callbacks_t : menu_layout_t::command_requested_callbacks_t
  {
    std::function<void(const std::string& /*login*/, const std::string& /*pwd*/)> login;
  };

public:
  explicit app_t(const command_requested_callbacks_t& callback);
  ~app_t();

  void server_status_changed(bool server_online);
  template<typename T>
  void process(T&& msg)
  {
    message_processor_.process(std::forward<T>(msg));
  }

private:
  menu_layout_t menu_layout_;
  board_t board_;
  login_input_t login_input_;
  message_processor_t message_processor_;
  QQmlApplicationEngine engine_;
};

} // namespace controller
