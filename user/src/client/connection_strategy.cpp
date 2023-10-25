#include "client/connection_strategy.hpp"

namespace chess::cl {

search_port_t::search_port_t(const endpoint_t& start, const int first_port, const int last_port)
  : endpoint_{start}
  , first_port_{first_port}
  , last_port_{last_port}
{
  endpoint_.port = first_port_ - 1;
}

void search_port_t::exec(endpoint_t& curr_endpoint)
{
  if (endpoint_.port >= last_port_)
  {
    endpoint_.port = first_port_;
    curr_endpoint = endpoint_;
    return;
  }

  ++endpoint_.port;
  curr_endpoint = endpoint_;
}

connect_port_t::connect_port_t(const endpoint_t& e)
  : endpoint_{e}
{}

void connect_port_t::exec(endpoint_t& curr_endpoint)
{
  curr_endpoint = endpoint_;
}

} // namespace chess::cl
