#pragma once

#include "client/connection_strategy.hpp"
#include <messages/messages.hpp>

#include <functional>
#include <memory>
#include <string>

namespace chess::cl {

template<typename T>
concept raw_to_server_msg = msg::one_of_to_server_msgs<T> && !msg::one_of<T, msg::some_datagram_t, msg::tokenized_msg_t, msg::is_server_lost_t, msg::message_received_t>;

class client_t
{
public:
  using message_received_callback_t = std::function<void(std::string)>;
  using server_status_changed_callback_t = std::function<void(bool /*online*/)>;
  using prepare_msg_strategy_t = std::function<std::string(msg::some_datagram_t data, uint64_t ser_num, uint64_t response_ser_num)>;

public:
  client_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed, const prepare_msg_strategy_t& prepare_msg_strategy,
           std::unique_ptr<connection_strategy_t> connection_strategy);
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  ~client_t();

  template<raw_to_server_msg T>
  void send(T&& data)
  {
    send(msg::some_datagram_t{std::forward<T>(data)});
  }

private:
  void send(msg::some_datagram_t data);

  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::cl
