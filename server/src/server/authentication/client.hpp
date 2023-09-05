#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"
#include "logic/credentials.hpp"
#include <spdlog/fmt/ostr.h>

namespace server::authentication {

struct client_t
{
  common::uuid_t uuid;
  endpoint_t address;
  logic::credentials_t credentials;
};

std::ostream& operator<<(std::ostream& os, const client_t& cl);

} // namespace server::authentication
