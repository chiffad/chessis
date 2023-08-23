#pragma once

#include "common/helper.hpp"
#include "common/unique_id_generator.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <string>

#include "spdlog/fmt/ostr.h"

namespace server {

class client_t
{
public:
  using uuid_t = common::uuid_t;

  client_t(io_service_t& io_serv, const endpoint_t& addr, const uuid_t& uuid);
  ~client_t();
  void push_from_server(const std::string& message);
  void push_for_send(const std::string& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();
  endpoint_t get_address() const;
  const uuid_t& uuid() const;

  void set_login_pwd(const std::string& log, const std::string& pwd);
  std::string get_login() const;
  std::string get_pwd() const;
  void set_rating(const int rating);
  int get_rating() const;
  bool playing_white() const;
  void set_playing_white(bool playing_white);

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;
  client_t(client_t&&) = default;
  client_t& operator=(client_t&&) = default;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

std::ostream& operator<<(std::ostream& os, const client_t& c);
bool operator==(const client_t& lhs, const client_t& rhs);

} // namespace server
