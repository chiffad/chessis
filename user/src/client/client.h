#pragma once

#include <functional>
#include <memory>
#include <string>

namespace cl {

class client_t
{
public:
  using message_received_callback_t = std::function<void(std::string)>;
  using server_status_changed_callback_t = std::function<void(bool /*online*/)>;

public:
  client_t(const message_received_callback_t& callback, const server_status_changed_callback_t& server_status_changed);
  ~client_t();
  void send(const std::string& message);

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl_;
};

} // namespace cl
