#pragma once

#include "common/helper.hpp"
#include <messages/messages.hpp>
#include <spdlog/fmt/ostr.h>
#include <string>

namespace chess::server {

template<typename T>
struct datagram_t
{
  endpoint_t address;
  T message;
};

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const datagram_t<T>& d)
{
  return os << "Datagram{ address=" << d.address << "; message=" << d.message << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const datagram_t<msg::some_datagram_t>& d)
{
  return os << "Datagram{ address=" << d.address << "; message={ type=" << d.message.type << "; msg=" << d.message.data << "; }";
}

} // namespace chess::server