#pragma once

#include <boost/asio.hpp>

namespace chess {
using io_service_t = boost::asio::io_service;
using endpoint_t = boost::asio::ip::udp::endpoint;
using error_code_t = boost::system::error_code;
using deadline_timer_t = boost::asio::deadline_timer;
} // namespace chess