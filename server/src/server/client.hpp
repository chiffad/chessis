#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "spdlog/fmt/ostr.h"

namespace server {

class client_t
{
public:
  client_t(io_service_t& io_serv, const endpoint_t& addr);
  ~client_t();
  void message_received(const std::string& message);
  void push_for_send(const std::string& message);
  bool message_for_server_append() const;
  bool message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();

  const endpoint_t& address() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  client_t(client_t&&) = default;
  client_t& operator=(client_t&&) = default;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

std::ostream& operator<<(std::ostream& os, const client_t& c);
bool operator==(const client_t& lhs, const client_t& rhs);

} // namespace server
