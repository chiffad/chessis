#include "logic/credentials.hpp"

namespace chess::logic {

std::ostream& operator<<(std::ostream& os, const credentials_t& c)
{
  return os << "Credentials{ login=" << c.login << "; pwd=" << c.pwd << " }";
}

bool operator==(const credentials_t& lhs, const credentials_t& rhs)
{
  return lhs.login == rhs.login && lhs.pwd == rhs.pwd;
}

bool operator!=(const credentials_t& lhs, const credentials_t& rhs)
{
  return !(lhs == rhs);
}

} // namespace chess::logic