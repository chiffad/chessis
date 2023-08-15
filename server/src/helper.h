#pragma once

#include <boost/asio.hpp>
#include <boost/mpl/begin_end.hpp>
#include <exception>
#include <sstream>
#include <string>

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

using io_service_t = boost::asio::io_service;
using endpoint_t = boost::asio::ip::udp::endpoint;
using error_code_t = boost::system::error_code;
using deadline_timer_t = boost::asio::deadline_timer;

namespace server::helper {
namespace detail {
template<typename OS, typename T>
void get1(OS& ostream, const T& _1)
{
  ostream << _1;
}

template<typename OS, typename T, typename... Args>
void get1(OS& ostream, const T& _1, const Args&... args)
{
  ostream << _1;

  get1(ostream, args...);
}

template<typename... Args>
std::string get(const Args&... args)
{
  std::ostringstream ss;
  detail::get1(ss, args...);
  return ss.str();
}
} // namespace detail

template<typename... Args>
void exception_fn(const Args&... args)
{
  throw std::logic_error(detail::get(args...));
}

#define throw_except(...) exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)

} // namespace server::helper
