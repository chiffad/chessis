#pragma once

#include "common/helper.hpp"
#include "server/client.hpp"
#include "server/datagram.hpp"

#include <map>
#include <memory>

namespace server {

using clients_arr_t = std::map<endpoint_t, client_t>;

class clients_holder_t : private clients_arr_t
{
public:
  explicit clients_holder_t(io_service_t& io_service);
  void add(const endpoint_t& addr);
  void process(const datagram_t& datagram);
  std::vector<datagram_t> datagrams_to_send();
  std::vector<datagram_t> datagrams_to_process();
  // clients_arr_t::iterator find(const endpoint_t& addr);
  // clients_arr_t::iterator find(const std::string& login);

  using clients_arr_t::at;
  using clients_arr_t::begin;
  using clients_arr_t::end;
  using clients_arr_t::find;

private:
  io_service_t& io_service_;
};

} // namespace server