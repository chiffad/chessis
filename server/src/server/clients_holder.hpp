#pragma once

#include "common/helper.hpp"
#include "server/client.hpp"

#include <map>
#include <memory>

namespace server {

using clients_arr_t = std::map<client_t::uuid_t, client_t>;

class clients_holder_t : private clients_arr_t
{
public:
  explicit clients_holder_t(io_service_t& io_service);
  client_t::uuid_t add(const endpoint_t& addr);
  clients_arr_t::iterator find(const endpoint_t& addr);
  clients_arr_t::iterator find(const std::string& login);

  using clients_arr_t::at;
  using clients_arr_t::begin;
  using clients_arr_t::end;

private:
  io_service_t& io_service_;
  common::uuid_generator_t uuid_generator_;
};

} // namespace server