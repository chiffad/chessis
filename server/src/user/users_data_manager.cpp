#include "user/users_data_manager.hpp"
#include "common/unique_id_generator.hpp"

#include <map>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>

namespace chess::server::user {

namespace {

inline bool format_valid(const credentials_t& cred)
{
  return !cred.login.empty() && !cred.pwd.empty();
}

template<typename T>
std::string to_string(const T& data)
{
  std::ostringstream ss;
  ss << data;
  return ss.str();
}

} // namespace

struct users_data_manager_t::impl_t
{
  std::map<client_uuid_t, user_data_t> uuid_to_user_;
  std::map<std::string, client_uuid_t> login_to_uuid_;

  common::uuid_generator_t user_uuid_generator_;
};

users_data_manager_t::users_data_manager_t()
  : impl_{std::make_unique<impl_t>()}
{}

users_data_manager_t::~users_data_manager_t() = default;

users_data_manager_t::known_user_res_t users_data_manager_t::known(const credentials_t& credentials) const
try
{
  if (!format_valid(credentials))
  {
    return known_user_res_t::wrong_format;
  }

  auto uuid_it = impl_->login_to_uuid_.find(credentials.login);
  if (uuid_it == impl_->login_to_uuid_.end()) return known_user_res_t::unknown;

  return impl_->uuid_to_user_.at(uuid_it->second).credentials().pwd == credentials.pwd ? known_user_res_t::known : known_user_res_t::wrong_pwd;
}
catch (const std::exception& ex)
{
  SPDLOG_ERROR("Exception handled: {}; credentials={}", ex.what(), credentials);
  throw;
}

user_data_t& users_data_manager_t::add_user(const credentials_t& credentials)
{
  if (known(credentials) != known_user_res_t::unknown)
  {
    throw std::logic_error("Can not add user with credentials=" + to_string(credentials) + " as this user present!");
  }

  const auto uuid = impl_->user_uuid_generator_.new_uuid();
  impl_->login_to_uuid_[credentials.login] = uuid;
  auto res = impl_->uuid_to_user_.emplace(std::piecewise_construct, std::forward_as_tuple(uuid), std::forward_as_tuple(uuid, credentials));
  return res.first->second;
}

const user_data_t& users_data_manager_t::user(const std::string& login) const
try
{
  return impl_->uuid_to_user_.at(impl_->login_to_uuid_.at(login));
}
catch (const std::exception& ex)
{
  SPDLOG_ERROR("Exception handled: {}; login={}", ex.what(), login);
  throw std::logic_error("Can not return uuid for user with credentials=" + to_string(login) + " as this user not present!");
}

const user_data_t& users_data_manager_t::user(const client_uuid_t& uuid) const
try
{
  return impl_->uuid_to_user_.at(uuid);
}
catch (const std::exception& ex)
{
  SPDLOG_ERROR("Failed to find uuid={}", uuid);
  throw std::logic_error("Can not return credentials for user uuid=" + to_string(uuid) + " as this user not present!");
}

} // namespace chess::server::user
