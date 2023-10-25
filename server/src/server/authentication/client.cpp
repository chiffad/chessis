#include "server/authentication/client.hpp"

namespace chess::server::authentication {

std::ostream& operator<<(std::ostream& os, const client_t& cl)
{
  return os << "Authentication Client={ uuid=" << cl.uuid << "; address=" << cl.address << "; credentials=" << cl.credentials << " }";
}

} // namespace chess::server::authentication
