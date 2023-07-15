#pragma once

#include "controller/board_graphic.h"
#include "controller/login_input.hpp"
#include "controller/message_processor.hpp"

#include <QQmlApplicationEngine>

namespace controller {

class app_t
{
public:
  app_t(QQmlApplicationEngine& engine, const board_graphic_t::command_requested_callback_t& callback);
  ~app_t();
  void process(const std::string& server_message);

private:
  board_graphic_t board_;
  login_input_t login_input_;
  message_processor_t message_processor_;
};

} // namespace controller