#pragma once

#include "common/helper.hpp"
#include "server/datagram.hpp"
#include "server/logic/client.hpp"

#include <map>
#include <memory>
#include <vector>

namespace chess::server::logic {

using clients_arr_t = std::map<endpoint_t, client_t>;

class clients_holder_t : private clients_arr_t
{
public:
  using connection_status_signal_t = boost::signals2::signal<void(const client_t&, bool)>;

public:
  clients_holder_t(io_service_t& io_service, const connection_status_signal_t::slot_type& subscriber);
  void add(const endpoint_t& addr);
  void process(const datagram_t<std::string>& datagram);
  std::vector<datagram_t<std::string>> datagrams_to_send();
  std::vector<datagram_t<msg::some_datagram_t>> datagrams_to_process();

  using clients_arr_t::at;
  using clients_arr_t::begin;
  using clients_arr_t::end;
  using clients_arr_t::find;

private:
  io_service_t& io_service_;
  connection_status_signal_t::slot_type subscriber_;
};

} // namespace chess::server::logic