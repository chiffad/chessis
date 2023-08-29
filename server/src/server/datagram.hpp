#pragma once

#include "common/helper.hpp"
#include <spdlog/fmt/ostr.h>
#include <string>

namespace server {

struct datagram_t
{
  endpoint_t address;
  std::string message;
};

inline std::ostream& operator<<(std::ostream& os, const datagram_t& d)
{
  return os << "Datagram{ address=" << d.address << "; message=" << d.message << "; }";
}

} // namespace server