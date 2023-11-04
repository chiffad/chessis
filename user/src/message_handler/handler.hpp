#pragma once

#include <memory>
#include <messages/messages.hpp>
#include <string>

namespace chess::controller {
class app_t;
}
namespace chess::cl {
class client_controller_t;
}

namespace chess::message_handler {

class handler_t
{
public:
  explicit handler_t(controller::app_t& app_controller, cl::client_controller_t& client);
  ~handler_t();
  void handle(msg::some_datagram_t msg);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::message_handler