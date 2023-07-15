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

constexpr const char* const START_ERROR_MESSAGE = "Enter login/password";
} // namespace

namespace controller {

login_input_t::login_input_t(const login_entered_callback_t& callback)
  : login_{}
  , pwd_{}
  , login_valid_{false}
  , pwd_valid_{false}
  , error_message_{START_ERROR_MESSAGE}
  , login_entered_callback_{callback}
{}

login_input_t::~login_input_t() = default;

QString login_input_t::login() const
{
  return login_;
}

void login_input_t::set_login(const QString& l)
{
  set_if_not_eq(login_, l, [&]() {
    SPDLOG_INFO("New login={}", login_);
    emit login_changed();
  });
}

QString login_input_t::pwd() const
{
  return pwd_;
}

void login_input_t::set_pwd(const QString& pwd)
{
  set_if_not_eq(pwd_, pwd, [&]() {
    SPDLOG_INFO("New pwd={}", pwd_);
    emit pwd_changed();
  });
}

bool login_input_t::login_valid() const
{
  return login_valid_;
}

void login_input_t::set_login_valid(const bool valid)
{
  set_if_not_eq(login_valid_, valid, [&]() { emit login_valid_changed(); });
}

bool login_input_t::pwd_valid() const
{
  return pwd_valid_;
}

void login_input_t::set_pwd_valid(const bool valid)
{
  set_if_not_eq(pwd_valid_, valid, [&]() { emit pwd_valid_changed(); });
}

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
  for (auto i : login)
  {
    if (!i.isLetterOrNumber())
    {
      return false;
    }
  }

  for (auto i : pwd)
  {
    if (!i.isLetterOrNumber())
    {
      return false;
    }
  }

  login_entered_callback_(login.toStdString(), pwd.toStdString());
  return true;
}
} // namespace controller
