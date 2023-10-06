#pragma once

#include <memory>
#include <string>

namespace controller {
class app_t;
}
namespace cl {
class client_controller_t;
}

namespace message_handler {

class handler_t
{
public:
  explicit handler_t(controller::app_t& app_controller, cl::client_controller_t& client);
  ~handler_t();
  void handle(std::string msg);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace message_handler