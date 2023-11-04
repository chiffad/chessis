#include "client/client_controller.hpp"

#include <messages/messages.hpp>
#include <messages/messages_io.hpp>
#include <spdlog/spdlog.h>

namespace chess::cl {

namespace {

std::unique_ptr<cl::connection_strategy_t> build_search_port_strategy()
{
  return std::make_unique<cl::search_port_t>(cl::endpoint_t{"127.0.0.1", cl::login_server::FIRST_PORT}, cl::login_server::FIRST_PORT, cl::login_server::LAST_PORT);
}

std::unique_ptr<cl::connection_strategy_t> build_connect_port_strategy(std::string address, uint16_t port)
{
  return std::make_unique<cl::connect_port_t>(cl::endpoint_t{std::move(address), port});
}

client_t::prepare_msg_strategy_t build_authentificaton_client_prepare_msg_strategy()
{
  return [](msg::some_datagram_t data, uint64_t ser_num, uint64_t response_ser_num) {
    return msg::prepare_for_send(msg::incoming_datagram_t{std::move(data), ser_num, response_ser_num});
  };
}

client_t::prepare_msg_strategy_t build_logic_client_prepare_msg_strategy()
{
  return build_authentificaton_client_prepare_msg_strategy();
}

std::unique_ptr<client_t> build_authentificaton_client(const client_t::message_received_callback_t& callback,
                                                       const client_t::server_status_changed_callback_t& server_status_changed)
{
  return std::make_unique<client_t>(callback, server_status_changed, build_authentificaton_client_prepare_msg_strategy(), build_search_port_strategy());
}

std::unique_ptr<client_t> build_logic_client(const client_t::message_received_callback_t& callback, const client_t::server_status_changed_callback_t& server_status_changed,
                                             std::string logic_server_address, uint16_t port)
{
  return std::make_unique<client_t>(callback, server_status_changed, build_logic_client_prepare_msg_strategy(), build_connect_port_strategy(std::move(logic_server_address), port));
}

} // namespace

struct client_controller_t::impl_t
{
  impl_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed)
    : message_received_callback_{callback}
    , server_status_changed_callback_{server_status_changed}
    , client_{build_authentificaton_client(message_received_callback_, server_status_changed_callback_)}
  {}

  /*template<msg::one_of_to_server_msgs Msg_t, typename... Args>
  inline void send(Args&&... args)
  {
    client_->send(prepare_for_send<Msg_t>(std::forward<Args>(args)...));
  }*/

  /*template<tokenized_msg_to_server Msg_t, typename... Args>
  inline client_t::data_to_send_t prepare_for_send(Args&&... args) const
  {
    if (!token_)
    {
      SPDLOG_CRITICAL("there is no token!");
      throw std::logic_error("prepare_for_send with token requested, but there is no token!");
    }
    return client_t::data_to_send_t{Msg_t{std::forward<Args>(args)..., *token_}};
  }

  template<msg::one_of_to_server_msgs Msg_t, typename... Args>
  inline client_t::data_to_send_t prepare_for_send(Args&&... args) const
  {
    return client_t::data_to_send_t{Msg_t{std::forward<Args>(args)...}};
  }*/

  const message_received_callback_t message_received_callback_;
  const server_status_changed_callback_t server_status_changed_callback_;
  std::unique_ptr<client_t> client_;
  std::unique_ptr<msg::token_t> token_;
};

client_controller_t::client_controller_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed)
  : impl_{std::make_unique<impl_t>(callback, server_status_changed)}
{}

client_controller_t::~client_controller_t() = default;

void client_controller_t::process(msg::login_response_t data)
{
  SPDLOG_INFO("New login info received: ", data);
  impl_->token_ = std::make_unique<msg::token_t>(std::move(data.token));
  impl_->client_ = build_logic_client(impl_->message_received_callback_, impl_->server_status_changed_callback_, std::move(data.logic_server_address), data.logic_server_port);
}

void client_controller_t::send_login(std::string login, std::string pwd)
{
  impl_->client_->send(msg::login_t{std::move(login), std::move(pwd)});
}
void client_controller_t::send_move(std::string data)
{
  impl_->client_->send(msg::move_t{std::move(data)});
}
void client_controller_t::send_go_to_history(uint16_t history_i)
{
  impl_->client_->send(msg::go_to_history_t{history_i});
}
void client_controller_t::send_new_game()
{
  impl_->client_->send(msg::new_game_t{});
}
void client_controller_t::send_back_move()
{
  impl_->client_->send(msg::back_move_t{});
}
void client_controller_t::send_opponent_inf()
{
  impl_->client_->send(msg::opponent_inf_t{});
}
void client_controller_t::send_my_inf()
{
  impl_->client_->send(msg::my_inf_t{});
}

} // namespace chess::cl
