#ifndef __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__
#define __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__

#include <QString>
#include <QTextStream>
#include <QDebug>
#include <exception>


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
        ostream<< _1;
        log_fn1(ostream, args...);
    }
  } // namespace detail

  template<typename... Args>
  void exception_fn(Args... args)
  {
    QString str;
    QTextStream stream(&str);

    detail::log_fn1(stream, args...);

    throw std::logic_error(str.toStdString());
  }

  template<typename... Args>
  void log_fn(Args... args)
  {
    QString str;
    QTextStream stream(&str);

    detail::log_fn1(stream, args...);

    qDebug()<<str;
  }

  #define throw_exception(...)  exception_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)
  #define log(...)  log_fn(__FILE__, "(", __LINE__, "): ", __VA_ARGS__)
} //namespace sc

#endif // __MY_LOG_H__KAJSHDJKAHWJKEJKNASJKDJKASHDJKAHSOIOAWJDKASJDWJEOQIJEOQIWJ__