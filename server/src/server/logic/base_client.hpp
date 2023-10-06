#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"
#include <messages/messages.hpp>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <memory>
#include <optional>
#include <string>

#include "spdlog/fmt/ostr.h"

namespace server::logic {

class base_client_t
{
public:
  using connection_status_signal_t = boost::signals2::signal<void(bool)>;

public:
  base_client_t(io_service_t& io_serv, const endpoint_t& addr);
  base_client_t(const base_client_t&) = delete;
  base_client_t& operator=(const base_client_t&) = delete;
  base_client_t(base_client_t&&) = default;
  base_client_t& operator=(base_client_t&&) = default;
  virtual ~base_client_t();

  bool message_for_send_append() const;
  bool message_for_logic_append() const;
  std::string pull_for_send();
  msg::some_datagram_t pull_for_logic();

  const endpoint_t& address() const;
  bool online() const;
  boost::signals2::connection connect_connection_status_changed(const connection_status_signal_t::slot_type& subscriber);

protected:
  std::optional<msg::some_datagram_t> preprocess_message(const std::string& m);
  void add_for_send(const std::string& message, bool extra_message = false);
  void add_for_logic(msg::some_datagram_t);

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

std::ostream& operator<<(std::ostream& os, const base_client_t& c);
bool operator==(const base_client_t& lhs, const base_client_t& rhs);

} // namespace server
