#pragma once

#include <QUdpSocket>
#include <functional>
#include <string>

#include "spdlog/fmt/ostr.h"

namespace chess::cl {

struct endpoint_t
{
  std::string host{};
  quint16 port{};
};

struct data_t
{
  endpoint_t endpoint{};
  std::string data{};
};

class udp_socket_t
{
public:
  explicit udp_socket_t(const std::function<void()>& ready_read);
  bool bind();
  endpoint_t endpoint() const;
  data_t read();
  void write(const data_t& data);

private:
  QUdpSocket socket_;
};

bool operator==(const endpoint_t& lhs, const endpoint_t& rhs);

std::ostream& operator<<(std::ostream& os, const endpoint_t& e);
std::ostream& operator<<(std::ostream& os, const data_t& d);

} // namespace chess::cl