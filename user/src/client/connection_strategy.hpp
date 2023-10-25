#pragma once

#include "client/socket.hpp"

namespace chess::cl {

struct connection_strategy_t
{
  virtual ~connection_strategy_t() = default;
  virtual void exec(endpoint_t&) = 0;
};

namespace login_server {
const int FIRST_PORT = 49152;
const int LAST_PORT = 49300;
} // namespace login_server

struct search_port_t : connection_strategy_t
{
  search_port_t(const endpoint_t& start, const int first_port, const int last_port);
  void exec(endpoint_t& curr_endpoint) override;

private:
  endpoint_t endpoint_;
  const int first_port_;
  const int last_port_;
};

struct connect_port_t : connection_strategy_t
{
  explicit connect_port_t(const endpoint_t& endpoint);
  void exec(endpoint_t& curr_endpoint) override;

private:
  endpoint_t endpoint_;
};

} // namespace chess::cl