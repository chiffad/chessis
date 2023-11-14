#include "user_data/users_data_manager.hpp"
#include "common/unique_id_generator.hpp"

#include <map>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>

namespace chess::server::user_data {

namespace {

struct credentials_less_by_login_t
{
  bool operator()(const credentials_t& lhs, const credentials_t& rhs) const { return lhs.login < rhs.login; }
};

inline bool format_valid(const credentials_t& cred)
{
  return !cred.login.empty() && !cred.pwd.empty();
}

std::string to_string(const credentials_t& credentials)
{
  std::ostringstream ss;
  ss << credentials;
  return ss.str();
}

} // namespace

struct users_data_manager_t::impl_t
{
  std::map<credentials_t, client_uuid_t, credentials_less_by_login_t> clients_;
  common::uuid_generator_t clients_uuid_generator_;
};

users_data_manager_t::users_data_manager_t()
  : impl_{std::make_unique<impl_t>()}
{}

users_data_manager_t::~users_data_manager_t() = default;

users_data_manager_t::known_user_res_t users_data_manager_t::known(const credentials_t& credentials) const
try
{
  if(!format_valid(credentials))
  {
    return known_user_res_t::wrong_format;
  }

  auto it = impl_->clients_.find(credentials);
  if (it == impl_->clients_.end()) return known_user_res_t::unknown;

  if (it->first.pwd != credentials.pwd) return known_user_res_t::wrong_pwd;

  return known_user_res_t::known;
}
catch (const std::exception& ex)
{
  SPDLOG_ERROR("Exception handled: {}; credentials={}", ex.what(), credentials);
  throw;
}

client_uuid_t users_data_manager_t::add_user(const credentials_t& credentials)
{
  if (known(credentials) != known_user_res_t::unknown)
  {
    throw std::logic_error("Can not add user with credentials=" + to_string(credentials) + " as this user present!");
  }

  auto uuid = impl_->clients_uuid_generator_.new_uuid();
  impl_->clients_[credentials] = std::move(uuid);
  return uuid;
}

client_uuid_t users_data_manager_t::uuid(const credentials_t& credentials) const
try
{
  return impl_->clients_.at(credentials);
}
catch (const std::exception& ex)
{
  SPDLOG_ERROR("Exception handled: {}; credentials={}", ex.what(), credentials);
  throw std::logic_error("Can not return uuid for user with credentials=" + to_string(credentials) + " as this user not present!");
}

} // namespace chess::server::user_data
