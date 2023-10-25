#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/fmt/ostr.h>

namespace chess {
using io_service_t = boost::asio::io_service;
using endpoint_t = boost::asio::ip::udp::endpoint;
using error_code_t = boost::system::error_code;
using deadline_timer_t = boost::asio::deadline_timer;
using client_uuid_t = boost::uuids::uuid;

} // namespace chess

namespace boost::uuids {
inline std::ostream& operator<<(std::ostream& os, const uuid& id)
{
  return os << boost::uuids::to_string(id);
}
} // namespace boost::uuids
