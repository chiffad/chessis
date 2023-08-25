#pragma once

#include "spdlog/fmt/ostr.h"
#include <string>

namespace logic {

struct credentials_t
{
  std::string login;
  std::string pwd;
};

std::ostream& operator<<(std::ostream& os, const credentials_t& c);
bool operator==(const credentials_t& lhs, const credentials_t& rhs);
bool operator!=(const credentials_t& lhs, const credentials_t& rhs);

} // namespace logic
