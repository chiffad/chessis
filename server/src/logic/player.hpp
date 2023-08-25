#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"
#include "logic/credentials.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "spdlog/fmt/ostr.h"

namespace logic {

class player_t
{
public:
  using uuid_t = common::uuid_t;

  player_t(const endpoint_t& addr, const uuid_t& uuid);
  ~player_t();

  const endpoint_t& address() const;
  const uuid_t& uuid() const;
  void set_credentials(const credentials_t& cred);
  const credentials_t& credentials() const;
  void set_rating(const int rating);
  int rating() const;
  bool playing_white() const;
  void set_playing_white(bool playing_white);

public:
  player_t(const player_t&) = delete;
  player_t& operator=(const player_t&) = delete;
  player_t(player_t&&) = default;
  player_t& operator=(player_t&&) = default;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

std::ostream& operator<<(std::ostream& os, const player_t& c);
bool operator==(const player_t& lhs, const player_t& rhs);

} // namespace logic
