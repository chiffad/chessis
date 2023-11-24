#include "user/user_data.hpp"

namespace chess::server::user {

user_data_t::user_data_t(client_uuid_t uuid, credentials_t credentials)
  : elo_rating_{1200}
  , uuid_{std::move(uuid)}
  , credentials_{std::move(credentials)}
{}

const client_uuid_t& user_data_t::uuid() const
{
  return uuid_;
}

void user_data_t::set_elo_rating(const uint32_t rating)
{
  elo_rating_ = rating;
}

uint32_t user_data_t::elo_rating() const
{
  return elo_rating_;
}

const credentials_t& user_data_t::credentials() const
{
  return credentials_;
}

std::ostream& operator<<(std::ostream& os, const user_data_t& c)
{
  return os << "Player{ uuid=" << c.uuid() << "; credentials=" << c.credentials() << "; elo_rating=" << c.elo_rating() << " }";
}

bool operator==(const user_data_t& lhs, const user_data_t& rhs)
{
  return lhs.uuid() == rhs.uuid();
}

} // namespace chess::server::user
