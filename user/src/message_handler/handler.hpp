#pragma once

#include "controller/app.hpp"
#include <memory>

namespace message_handler {

class handler_t
{
public:
  explicit handler_t(controller::app_t& app_controller);
  ~handler_t();
  void handle(std::string msg);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace message_handler