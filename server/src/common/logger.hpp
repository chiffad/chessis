#pragma once

namespace chess::logger {

class logger_t
{
public:
  static logger_t& get();
  void init();
  ~logger_t();

private:
  logger_t() = default;
  logger_t(const logger_t&) = default;
  logger_t(logger_t&&) = default;
  logger_t& operator=(const logger_t&) = default;
  logger_t& operator=(logger_t&&) = default;
};

} // namespace chess::logger
