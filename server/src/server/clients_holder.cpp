#include "server/clients_holder.hpp"

namespace server {

clients_holder_t::clients_holder_t(io_service_t& io_service)
  : io_service_(io_service)
{}

client_t& clients_holder_t::add(const endpoint_t& addr)
{
  push_back(std::make_shared<client_t>(io_service_, addr));
  return *back();
}

clients_arr_t::iterator clients_holder_t::get(const endpoint_t& addr)
{
  return std::find_if(begin(), end(), [&addr](const auto& cl) { return cl->get_address() == addr; });
}

clients_arr_t::iterator clients_holder_t::get(const std::string& login)
{
  return std::find_if(begin(), end(), [&login](const auto& cl) { return cl->get_login() == login; });
}

} // namespace server