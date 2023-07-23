#pragma once

#include "controller/board_graphic.h"
#include "controller/login_input.hpp"
#include "controller/menu_layout.hpp"
#include "controller/message_processor.hpp"

#include <QQmlApplicationEngine>

namespace controller {

class app_t
{
public:
  explicit app_t(const board_graphic_t::command_requested_callback_t& callback);
  ~app_t();
  void process(const std::string& server_message);

private:
  menu_layout_t menu_layout_;
  board_graphic_t board_;
  login_input_t login_input_;
  message_processor_t message_processor_;
  QQmlApplicationEngine engine_;
};

} // namespace controller