#pragma once

#include "client/connection_strategy.hpp"
#include <messages/messages.hpp>

#include <functional>
#include <memory>
#include <string>

namespace cl {

class client_t
{
public:
  struct data_to_send_t
  {
    template<msg::one_of_to_server_msgs T>
    data_to_send_t(T&& d)
      : data_{msg::prepare_for_send(d)}
    {}
    data_to_send_t() = default;
    data_to_send_t(const data_to_send_t&) = default;
    data_to_send_t(data_to_send_t&&) = default;
    data_to_send_t& operator=(const data_to_send_t&) = default;
    data_to_send_t& operator=(data_to_send_t&&) = default;

    inline const std::string& data() const { return data_; }

  private:
    std::string data_;
  };

public:
  using message_received_callback_t = std::function<void(std::string)>;
  using server_status_changed_callback_t = std::function<void(bool /*online*/)>;

public:
  client_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed, std::unique_ptr<connection_strategy_t> connection_strategy);
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  ~client_t();

  void send(const data_to_send_t& data);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace cl
