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
  using command_requested_callback_t = std::function<void(std::string)>;

public:
  explicit app_t(const command_requested_callback_t& callback);
  ~app_t();
  void process(const std::string& server_message);
  void server_status_changed(bool server_online);

private:
  menu_layout_t menu_layout_;
  board_t board_;
  login_input_t login_input_;
  message_processor_t message_processor_;
  QQmlApplicationEngine engine_;
};

} // namespace controller