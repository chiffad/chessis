#pragma once

#include "common/helper.hpp"
#include "logic/credentials.hpp"
#include <spdlog/fmt/ostr.h>

namespace chess::server::authentication {

struct client_t
{
  client_uuid_t uuid;
  endpoint_t address;
  logic::credentials_t credentials;
};

std::ostream& operator<<(std::ostream& os, const client_t& cl);

} // namespace chess::server::authentication
