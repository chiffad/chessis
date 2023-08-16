#pragma once

#include "helper.h"
#include "server/client.hpp"

#include <memory>
#include <vector>

namespace server {

using clients_arr_t = std::vector<std::shared_ptr<client_t>>;

class clients_holder_t : private clients_arr_t
{
public:
  explicit clients_holder_t(io_service_t& io_service);
  client_t& add(const endpoint_t& addr);
  clients_arr_t::iterator get(const endpoint_t& addr);
  clients_arr_t::iterator get(const std::string& login);

  using clients_arr_t::begin;
  using clients_arr_t::end;

private:
  io_service_t& io_service_;
};

} // namespace server