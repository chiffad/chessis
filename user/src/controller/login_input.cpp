#include "controller/login_input.hpp"

#include "helper.h"
#include <spdlog/spdlog.h>

namespace {
template<typename T1, typename T2, typename Fn>
inline void set_if_not_eq(T1& to_set, T2&& new_val, Fn&& call_if_set)
{
  if (to_set == new_val) return;

  to_set = std::forward<T2>(new_val);
  call_if_set();
}

constexpr const char* const ONLY_LETTER_AND_NUMS_ACCEPTER_ERR = "Only letters and numbers accepted!";
constexpr const char* const EMPTY_ERR = "PWD and login should not be empty!";
constexpr const char* const NO_ERROR = "";
} // namespace

namespace chess::controller {

login_input_t::login_input_t(const login_entered_callback_t& callback)
  : error_message_{}
  , login_entered_callback_{callback}
{}

login_input_t::~login_input_t() = default;

QString login_input_t::error_message() const
{
  return error_message_;
}

void login_input_t::set_error_message(const QString& str)
{
  set_if_not_eq(error_message_, str, [&]() { emit error_message_changed(); });
}

bool login_input_t::set_login(const QString& login, const QString& pwd)
{
  if (login.isEmpty() || pwd.isEmpty())
  {
    get_login(EMPTY_ERR);
    return false;
  }

  for (auto i : login)
  {
    if (!i.isLetterOrNumber())
    {
      get_login(ONLY_LETTER_AND_NUMS_ACCEPTER_ERR);
      return false;
    }
  }

  for (auto i : pwd)
  {
    if (!i.isLetterOrNumber())
    {
      get_login(ONLY_LETTER_AND_NUMS_ACCEPTER_ERR);
      return false;
    }
  }

  set_error_message(NO_ERROR);
  login_entered_callback_(login.toStdString(), pwd.toStdString());

  return true;
}

void login_input_t::get_login(const QString& error_mess)
{
  set_error_message(error_mess);
  emit enter_login();
}

} // namespace chess::controller
