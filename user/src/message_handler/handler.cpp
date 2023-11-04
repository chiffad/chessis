#include "message_handler/handler.hpp"

#include "client/client_controller.hpp"
#include "controller/app.hpp"
#include "message_handler/helper.hpp"

namespace chess::message_handler {

struct handler_t::impl_t
{
  impl_t(controller::app_t& app_controller, cl::client_controller_t& client)
    : app_controller_{app_controller}
    , client_{client}
  {}

  template<msg::one_of_to_client_msgs Msg>
  void handle(Msg&& msg)
  {
    app_controller_.process(std::forward<Msg>(msg));
  }

  void handle(msg::login_response_t login_response) { client_.process(std::move(login_response)); }

  controller::app_t& app_controller_;
  cl::client_controller_t& client_;
};

handler_t::handler_t(controller::app_t& app_controller, cl::client_controller_t& client)
  : impl_{std::make_unique<impl_t>(app_controller, client)}
{}

handler_t::~handler_t() = default;

void handler_t::handle(msg::some_datagram_t msg)
{
  helper::find_msg_type_and_handle(std::move(msg), *impl_);
}

} // namespace chess::message_handler
