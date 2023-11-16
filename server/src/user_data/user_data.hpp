#pragma once

#include "common/helper.hpp"
#include "user_data/credentials.hpp"

#include "spdlog/fmt/ostr.h"

namespace chess::server::user_data {

class user_data_t
{
public:
  user_data_t(client_uuid_t uuid, credentials_t credentials);

  const client_uuid_t& uuid() const;
  void set_elo_rating(uint32_t rating);
  uint32_t elo_rating() const;
  const credentials_t& credentials() const;

private:
  uint32_t elo_rating_;
  const client_uuid_t uuid_;
  credentials_t credentials_;
};

std::ostream& operator<<(std::ostream& os, const user_data_t& c);
bool operator==(const user_data_t& lhs, const user_data_t& rhs);

} // namespace chess::server::user_data
