#include "logger.hpp"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

namespace {
const spdlog::level::level_enum DEFAULT_LOG_LVL = spdlog::level::trace;
}

namespace logger {

logger_t& logger_t::get()
{
  static logger_t logger;
  return logger;
}

logger_t::~logger_t()
{
  spdlog::shutdown();
}

void logger_t::init()
{
  constexpr int items = 8192;
  constexpr int threads = 1;
  spdlog::init_thread_pool(items, threads);
  spdlog::set_level(DEFAULT_LOG_LVL);
  spdlog::flush_on(spdlog::level::trace);
  spdlog::flush_every(std::chrono::seconds(5));

  spdlog::set_pattern("[%H:%M:%S.%f][%^%l%$]<%s:%#><%!> %v");
}

} // namespace logger
