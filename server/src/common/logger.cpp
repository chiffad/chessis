#include "common/logger.hpp"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

#include "spdlog/pattern_formatter.h"
#include <boost/algorithm/string.hpp>
#include <memory>
#include <string>
#include <vector>

namespace chess::logger {

namespace {
const spdlog::level::level_enum DEFAULT_LOG_LVL = spdlog::level::trace;

class filename_folder_name_flag_formatter_t : public spdlog::custom_flag_formatter
{
public:
  void format(const spdlog::details::log_msg& log_msg, const std::tm&, spdlog::memory_buf_t& dest) override
  {
    std::vector<std::string> path;
    boost::split(path, log_msg.source.filename, boost::is_any_of("/\\"));

    std::string filepath = ":" + std::to_string(log_msg.source.line);
    for (size_t i = path.size() - 1; i > 1; --i)
    {
      const bool last_part = path[i - 1] == "src" && path[i - 2] == "server";
      filepath.insert(0, last_part ? path[i] : "/" + path[i]);
      if (last_part) break;
    }

    dest.append(filepath.data(), filepath.data() + filepath.size());
  }

  std::unique_ptr<custom_flag_formatter> clone() const override { return spdlog::details::make_unique<filename_folder_name_flag_formatter_t>(); }
};

} // namespace

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

  auto formatter = std::make_unique<spdlog::pattern_formatter>();
  formatter->add_flag<filename_folder_name_flag_formatter_t>('*').set_pattern("[%H:%M:%S.%f][%^%l%$]<%*><%!> %v");
  spdlog::set_formatter(std::move(formatter));
}

} // namespace chess::logger
