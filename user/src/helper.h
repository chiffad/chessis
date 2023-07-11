#pragma once

#include <QByteArray>
#include <QString>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace cl::helper {
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

template<typename... Args>
void log_fn(const Args&... args)
{
  std::cout << detail::get(args...) << std::endl;
}

template<typename... Args>
std::string get_str(const Args&... args)
{
  return detail::get(args...);
}

//#define throw_exception(...) ::cl::helper::exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)
#define log(...) log_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)

} // namespace cl::helper

std::ostream& operator<<(std::ostream& os, const QString& str);
std::ostream& operator<<(std::ostream& os, const QByteArray& str);
