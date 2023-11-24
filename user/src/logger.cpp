
#include "logger.hpp"
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

namespace chess::logger {

namespace {
const spdlog::level::level_enum DEFAULT_LOG_LVL = spdlog::level::debug;
}

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

  qInstallMessageHandler(logger::qt_message_hanlder);
}

void qt_message_hanlder(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  const std::string str{context.function == nullptr ? "" : context.function};
  const auto source_loc = spdlog::source_loc{context.file, context.line, str.c_str()};
  switch (type)
  {
    case QtDebugMsg: spdlog::log(source_loc, spdlog::level::level_enum::debug, msg.toStdString()); break;
    case QtInfoMsg: spdlog::log(source_loc, spdlog::level::level_enum::info, msg.toStdString()); break;
    case QtWarningMsg: spdlog::log(source_loc, spdlog::level::level_enum::warn, msg.toStdString()); break;
    case QtCriticalMsg: spdlog::log(source_loc, spdlog::level::level_enum::err, msg.toStdString()); break;
    case QtFatalMsg: spdlog::log(source_loc, spdlog::level::level_enum::critical, msg.toStdString()); abort();
  }
}

} // namespace chess::logger
