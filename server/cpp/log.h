#ifndef __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__
#define __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__

#include <exception>
#include <string>
#include <sstream>
#include <iostream>


namespace sr
{
  namespace detail
  {
    template<typename OS, typename T>
    void log_fn1(OS& ostream, T _1)
    {
      ostream<< _1;
    }

    template<typename OS, typename T, typename... Args>
    void log_fn1(OS& ostream, T _1, Args... args)
    {
      ostream<<_1;

      log_fn1(ostream, args...);
    }

    template<typename... Args>
    std::string get(Args... args)
    {
      std::ostringstream ss;
      detail::log_fn1(ss, args...);
      return ss.str();
    }
  } // namespace detail

  template<typename... Args>
  void exception_fn(Args... args)
  {
    throw std::logic_error(detail::get(args...));
  }

  template<typename... Args>
  void log_fn(Args... args)
  {
    std::cout<<detail::get(args...)<<std::endl;
  }

  #define throw_except(...)  exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)
  #define log(...)  log_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)

} //namespace sc

#endif // __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__