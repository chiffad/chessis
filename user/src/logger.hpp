#pragma once

#include <QMessageLogContext>
#include <QString>
#include <QtGlobal>

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

void qt_message_hanlder(QtMsgType type, const QMessageLogContext& context, const QString& msg);

} // namespace chess::logger
