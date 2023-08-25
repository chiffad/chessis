#pragma once

#include "common/helper.hpp"
#include <string>

namespace server {

struct datagram_t
{
  endpoint_t address;
  std::string message;
};

} // namespace server