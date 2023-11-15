#pragma once

#include "common/helper.hpp"
#include "user_data/credentials.hpp"

namespace chess::server::user_data {

class users_data_manager_t
{
public:
  enum class known_user_res_t
  {
    unknown,
    wrong_pwd,
    wrong_format,
    known
  };

public:
  explicit users_data_manager_t();
  ~users_data_manager_t();

  known_user_res_t known(const credentials_t& credentials) const;
  client_uuid_t add_user(const credentials_t& credentials);
  client_uuid_t uuid(const credentials_t& credentials) const;
  client_uuid_t uuid(const std::string& login) const;
  const credentials_t& credentials(const client_uuid_t& uuid) const;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace chess::server::user_data
