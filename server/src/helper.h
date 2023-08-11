#ifndef __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__
#define __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__

#include <exception>
#include <sstream>
#include <string>

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

namespace sr {
namespace helper {
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
std::string get_str(const Args&... args)
{
  return detail::get(args...);
}

#define throw_except(...) exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)

} // namespace helper
} // namespace sr

#endif // __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__
