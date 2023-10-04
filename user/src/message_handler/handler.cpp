#include "message_handler/handler.hpp"
#include "message_handler/helper.hpp"

namespace message_handler {

struct handler_t::impl_t
{
  impl_t(controller::app_t& app_controller)
    : app_controller_{app_controller}
  {}

  template<msg::one_of_to_client_msgs Msg>
  void handle(Msg&& msg)
  {
    app_controller_.process(std::forward<Msg>(msg));
  }

  void handle(msg::login_response_t login_response)
  {
    // TODO
  }

  controller::app_t& app_controller_;
};

handler_t::handler_t(controller::app_t& app_controller)
  : impl_{std::make_unique<impl_t>(app_controller)}
{}

handler_t::~handler_t() = default;

void handler_t::handle(std::string msg)
{
  helper::find_msg_type_and_handle(msg, *impl_);
}

} // namespace message_handler
