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

} // namespace

struct client_controller_t::impl_t
{
  impl_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed)
    : message_received_callback_{callback}
    , server_status_changed_callback_{server_status_changed}
    , client_{build_authentificaton_client()}
  {}

  client_t::prepare_msg_strategy_t build_authentificaton_client_prepare_msg_strategy()
  {
    return [](msg::some_datagram_t data, uint64_t ser_num, uint64_t response_ser_num) {
      return msg::prepare_for_send(msg::incoming_datagram_t{std::move(data), ser_num, response_ser_num});
    };
  }

  client_t::prepare_msg_strategy_t build_logic_client_prepare_msg_strategy()
  {
    return [this](msg::some_datagram_t data, uint64_t ser_num, uint64_t response_ser_num) {
      return msg::prepare_for_send(msg::tokenized_msg_t{*token_, msg::incoming_datagram_t{std::move(data), ser_num, response_ser_num}});
    };
  }

  std::unique_ptr<client_t> build_authentificaton_client()
  {
    return std::make_unique<client_t>(message_received_callback_, server_status_changed_callback_, build_authentificaton_client_prepare_msg_strategy(),
                                      build_search_port_strategy());
  }

  std::unique_ptr<client_t> build_logic_client(std::string logic_server_address, uint16_t port)
  {
    return std::make_unique<client_t>(message_received_callback_, server_status_changed_callback_, build_logic_client_prepare_msg_strategy(),
                                      build_connect_port_strategy(std::move(logic_server_address), port));
  }

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
  impl_->client_ = impl_->build_logic_client(std::move(data.logic_server_address), data.logic_server_port);
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
