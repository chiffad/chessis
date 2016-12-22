#include "client.h"

#include <QObject>
#include <QTimer>
#include <vector>

#include "messages.h"
#include "log.h"


using namespace sr;

struct client_t::impl_t
{
  impl_t(const int _port, const QHostAddress& _ip);
  void push_from_server(std::string message);
  void push_for_send(const std::string& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();
  int get_port() const;
  QHostAddress get_ip() const;
  void set_login(const std::string& log);
  std::string get_login() const;
  void set_rating(const int rating);
  int get_rating() const;

  bool check_ser_num(std::string& m);
  void add_for_server(const std::string& message, bool is_extra_message = false);
  void add_for_server(const messages::MESSAGE r_mes, bool is_extra_message = false);
  bool is_message_received();
  void connection_timer_timeout();
  void begin_wait_receive(const std::string& message);
  std::string add_serial_num(const std::string& data, const int num) const;
  int cut_serial_num(std::string& data) const;

  struct server_mess_t
  {
    server_mess_t(const std::string& m, const bool is_extra);
    server_mess_t() = default;
    std::string message;
    bool is_extra;
  };

  std::vector<server_mess_t> messages_for_server;
  std::vector<std::string> messages_for_logic;
  std::string last_send_message;

  std::string login;
  int elo;

  QTimer response_timer;
  QTimer connection_timer;

  int received_serial_num;
  int send_serial_num;
  bool is_received;

  int port;
  QHostAddress ip;

  const char FREE_SPASE = ' ';
};


client_t::client_t(const int port, const QHostAddress& ip)
    : impl(std::make_unique<impl_t>(port, ip))
{
}

client_t::~client_t()
{
}

void client_t::push_from_server(const std::string& message)
{
  impl->push_from_server(message);
}

void client_t::push_for_send(const std::string& message)
{
  impl->push_for_send(message);
}

bool client_t::is_message_for_server_append() const
{
  return impl->is_message_for_server_append();
}

bool client_t::is_message_for_logic_append() const
{
  return impl->is_message_for_logic_append();
}

std::string client_t::pull_for_server()
{
  return impl->pull_for_server();
}

std::string client_t::pull_for_logic()
{
  return impl->pull_for_logic();
}

int client_t::get_port() const
{
  return impl->get_port();
}

QHostAddress client_t::get_ip() const
{
  return impl->get_ip();
}

void client_t::set_login(const std::string& log)
{
  impl->set_login(log);
}

std::string client_t::get_login() const
{
  return impl->get_login();
}

void client_t::set_rating(const int rating)
{
  impl->set_rating(rating);
}

int client_t::get_rating() const
{
  return impl->get_rating();
}

client_t::impl_t::impl_t(const int _port, const QHostAddress& _ip)
    : received_serial_num(1), send_serial_num(0), is_received(true), port(_port), ip(_ip)
{
  enum { RESPONSE_WAIT_TIME = 1500, CHECK_CONNECT_TIME = 7000};
  response_timer.setInterval(RESPONSE_WAIT_TIME);
  connection_timer.setInterval(CHECK_CONNECT_TIME);

  QObject::connect(&response_timer, &QTimer::timeout, [&](){ is_message_received(); });
  QObject::connect(&connection_timer, &QTimer::timeout, [&](){ connection_timer_timeout(); });
}

void client_t::impl_t::push_from_server(std::string m)
{
  log("push_from_server: ",m);

  if(!check_ser_num(m))
    { return; }

  ++received_serial_num;
  connection_timer.start();

  const auto type = std::stoi(m.substr(0, m.find(FREE_SPASE)));

  if(type == messages::MESSAGE_RECEIVED)
  {
    is_received = true;
    return;
  }

  add_for_server(messages::MESSAGE_RECEIVED);

  switch(type)
  {
    case messages::IS_SERVER_LOST:
      break;
    case messages::HELLO_SERVER:
      add_for_server(messages::GET_LOGIN);
      break;
    default:
      messages_for_logic.push_back(m);
  }
}

void client_t::impl_t::push_for_send(const std::string& m)
{
  messages_for_server.push_back(server_mess_t(m, false));
}

bool client_t::impl_t::is_message_for_server_append() const
{
  return (!messages_for_server.empty() && (is_received || (messages_for_server.front().is_extra && messages_for_server.front().message == last_send_message)));
}

bool client_t::impl_t::is_message_for_logic_append() const
{
  return !messages_for_logic.empty();
}

std::string client_t::impl_t::pull_for_server()
{
  log("pull_for_server");

  const auto& _1 = messages_for_server.front();
  const std::string m = add_serial_num(_1.message, _1.is_extra ? send_serial_num : ++send_serial_num);

  if(std::stoi(_1.message) != messages::MESSAGE_RECEIVED)
    { begin_wait_receive(_1.message); }

  messages_for_server.erase(messages_for_server.begin());

  return m;
}

std::string client_t::impl_t::pull_for_logic()
{
  log("pull_for_logic");

  const std::string m = messages_for_logic.front();
  messages_for_logic.erase(messages_for_logic.begin());

  return m;
}

int client_t::impl_t::get_port() const
{
  return port;
}

QHostAddress client_t::impl_t::get_ip() const
{
  return ip;
}

void client_t::impl_t::set_login(const std::string& log)
{
  login = log;
}

std::string client_t::impl_t::get_login() const
{
  return login;
}

void client_t::impl_t::set_rating(const int rating)
{
  elo = rating;
}

int client_t::impl_t::get_rating() const
{
  return elo;
}

bool client_t::impl_t::check_ser_num(std::string& m)
{
  const int serial_num = cut_serial_num(m);

  if(serial_num == received_serial_num - 1 && std::stoi(m) != messages::MESSAGE_RECEIVED)
  {
    connection_timer.start();
    add_for_server(messages::MESSAGE_RECEIVED, true);
    return false;
  }

  if(serial_num != received_serial_num)
  {
    log("check_ser_num: Warning! Wrong serial number!");
    return false;
  }

  return true;
}

void client_t::impl_t::add_for_server(const std::string& m, bool is_extra_message)
{
  log("add_for_server: ", m);
  auto _1 = server_mess_t(m, is_extra_message);
  if(is_extra_message)
    { messages_for_server.insert(messages_for_server.begin(), _1); }
  else
    { messages_for_server.push_back(_1); }
}

void client_t::impl_t::add_for_server(const messages::MESSAGE r_mes, bool is_extra_message)
{
  log("add_for_server messages::MESSAGE: ", r_mes);
  auto _1 = server_mess_t(std::to_string(r_mes), is_extra_message);
  if(is_extra_message)
    { messages_for_server.insert(messages_for_server.begin(), _1); }
  else
    { messages_for_server.push_back(_1); }
}

void client_t::impl_t::begin_wait_receive(const std::string& message)
{
  is_received = false;
  last_send_message = message;
  response_timer.start();
}

bool client_t::impl_t::is_message_received()
{
  static int num_of_restarts = 0;
  if(is_received)
  {
    response_timer.stop();
    num_of_restarts = 0;
  }
  else
  {
    add_for_server(last_send_message, true);

    if(std::stoi(last_send_message) == messages::IS_CLIENT_LOST || num_of_restarts == 3)
      { messages_for_logic.push_back(std::to_string(messages::CLIENT_LOST)); }

    ++num_of_restarts;
    response_timer.start();
  }
  return is_received;
}

void client_t::impl_t::connection_timer_timeout()
{
  add_for_server(messages::IS_CLIENT_LOST);
  connection_timer.stop();
}

std::string client_t::impl_t::add_serial_num(const std::string& data, const int num) const
{
  return std::string(std::to_string(num) + FREE_SPASE + data);
}

int client_t::impl_t::cut_serial_num(std::string& data) const
{
  std::string serial_num(data.begin(), (data.begin() + data.find(FREE_SPASE)));
  data.erase(0, data.find(FREE_SPASE) + 1);

  return std::stoi(serial_num);
}

client_t::impl_t::server_mess_t::server_mess_t(const std::string& m, const bool is_extra_message)
    : message(m), is_extra(is_extra_message)
{
}