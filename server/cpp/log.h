#ifndef __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__
#define __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__

#include <QString>
#include <QTextStream>
#include <QDebug>
#include <exception>
#include <string>


namespace sr
{
  namespace detail
  {
    template<typename OS, typename T>
    void log_fn1(OS& ostream, T _1)
    {
      ostream<< _1;
    }

    template<typename OS>
    void log_fn1(OS& ostream, const std::string& _1)
    {
      ostream<< QString::fromStdString(_1);
    }

    template<typename OS, typename T, typename... Args>
    void log_fn1(OS& ostream, T _1, Args... args)
    {
      ostream<<_1;

      log_fn1(ostream, args...);
    }

    template<typename... Args>
    QString get(Args... args)
    {
      QString str;
      QTextStream stream(&str);

      detail::log_fn1(stream, args...);
      return str;
    }
  } // namespace detail

  template<typename... Args>
  void exception_fn(Args... args)
  {
    throw std::logic_error(detail::get(args...).toStdString());
  }

  template<typename... Args>
  void log_fn(Args... args)
  {
    qDebug()<<detail::get(args...);
  }

  #define throw_except(...)  exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)
  #define log(...)  log_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)

} //namespace sc

#endif // __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__