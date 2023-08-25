#include "logic/player.hpp"

#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>

namespace logic {

struct player_t::impl_t
{
  impl_t(const endpoint_t& addr, const uuid_t& uuid)
    : elo_{1200}
    , playing_white_{true}
    , address_(addr)
    , uuid_{uuid}
  {}

  credentials_t creds_;
  int elo_;
  bool playing_white_;
  const uuid_t uuid_;
  endpoint_t address_;
};

player_t::player_t(const endpoint_t& addr, const uuid_t& uuid)
  : impl_(std::make_unique<impl_t>(addr, uuid))
{}

player_t::~player_t() = default;

const endpoint_t& player_t::address() const
{
  return impl_->address_;
}

const player_t::uuid_t& player_t::uuid() const
{
  return impl_->uuid_;
}

void player_t::set_credentials(const credentials_t& creds)
{
  impl_->creds_ = creds;
}

const credentials_t& player_t::credentials() const
{
  return impl_->creds_;
}

void player_t::set_rating(const int rating)
{
  impl_->elo_ = rating;
}

int player_t::rating() const
{
  return impl_->elo_;
}

bool player_t::playing_white() const
{
  return impl_->playing_white_;
}

void player_t::set_playing_white(bool playing_white)
{
  impl_->playing_white_ = playing_white;
}

std::ostream& operator<<(std::ostream& os, const player_t& c)
{
  return os << "Player{ uuid=" << boost::uuids::to_string(c.uuid()) << "; creds=" << c.credentials() << "; address" << c.address() << " }";
}

bool operator==(const player_t& lhs, const player_t& rhs)
{
  return lhs.uuid() == rhs.uuid();
}

} // namespace logic