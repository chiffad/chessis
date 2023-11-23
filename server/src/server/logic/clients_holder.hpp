#pragma once

#include "common/helper.hpp"
#include "server/datagram.hpp"
#include "server/logic/client.hpp"

#include <map>
#include <memory>
#include <vector>

namespace chess::server::logic {

class clients_holder_t
{
public:
  using connection_status_signal_t = boost::signals2::signal<void(const client_uuid_t&, bool)>;

public:
  clients_holder_t(io_service_t& io_service, const connection_status_signal_t::slot_type& subscriber);
  client_t& add(const client_uuid_t& uuid, const endpoint_t& addr);
  void message_received(const endpoint_t& e, const client_uuid_t& uuid, msg::incoming_datagram_t message);

  std::vector<datagram_t<std::string>> datagrams_to_send();
  std::map<client_uuid_t, std::vector<msg::some_datagram_t>> datagrams_to_process();

  client_t& at(const client_uuid_t& addr);
  const client_t& at(const client_uuid_t& uuid) const;

private:
  void client_connection_changed(const client_uuid_t& uuid, bool online);
  std::shared_ptr<client_t> add_client(const client_uuid_t& uuid, const endpoint_t& addr);

private:
  io_service_t& io_service_;
  std::map<client_uuid_t, std::shared_ptr<client_t>> uuid_to_cl_;
  connection_status_signal_t::slot_type subscriber_;
};

} // namespace chess::server::logic