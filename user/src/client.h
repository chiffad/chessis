#pragma once

#include <memory>
#include <string>

namespace cl {

class client_t
{
public:
  client_t();
  ~client_t();
  void send(const std::string& message);
  std::string pull();
  bool is_message_append() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace cl
