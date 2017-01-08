#include "client.h"

#include <vector>

#include "messages.h"
#include "helper.h"


using namespace sr;
typedef boost::asio::io_service io_service_t;
typedef boost::asio::ip::udp::endpoint endpoint_t;
typedef boost::asio::deadline_timer deadline_timer_t;

struct client_t::impl_t
{
  impl_t(io_service_t& io_serv, const endpoint_t& addr);
  void push_from_server(std::string message);
  void push_for_send(const std::string& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();
  endpoint_t get_address() const;

  void set_login(const std::string& log);
  std::string get_login() const;
  void set_rating(const int rating);
  int get_rating() const;

  bool check_ser_num(std::string& m);
  void add_for_server(const std::string& message, bool is_extra_message = false);
  void is_message_received();
  void connection_timer_timeout();
  void begin_wait_receive(const std::string& message);
  std::string add_serial_num(const std::string& data, const int num) const;
  int cut_serial_num(std::string& data) const;
  void start_connection_timer();
  void start_response_timer();

  struct server_mess_t
  {
    server_mess_t(const std::string& m, const bool is_extra);
    server_mess_t() = default;
    std::string message;
    bool is_extra;
  };

  deadline_timer_t response_timer;
  deadline_timer_t connection_timer;

  std::vector<server_mess_t> messages_for_server;
  std::vector<std::string> messages_for_logic;
  std::string last_send_message;

  std::string login;
  int elo = 1200;

  int received_serial_num;
  int send_serial_num;
  bool is_received;

  endpoint_t address;

  const char FREE_SPASE = ' ';
};

client_t::client_t(io_service_t& io_serv, const endpoint_t& addr)
    : impl(std::make_unique<impl_t>(io_serv, addr))
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

endpoint_t client_t::get_address() const
{
  return impl->get_address();
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

client_t::impl_t::impl_t(io_service_t& io_serv, const endpoint_t& addr)
    : response_timer(io_serv), connection_timer(io_serv), received_serial_num(1), send_serial_num(0), is_received(true), address(addr)
{
}

void client_t::impl_t::push_from_server(std::string m)
{
  helper::log("push_from_server: ",m);

  if(!check_ser_num(m))
    { return; }

  ++received_serial_num;
  start_connection_timer();

  const auto type = msg::get_msg_type(m);

  if(type == msg::get_type<msg::message_received_t>::value)
  {
    is_received = true;
    return;
  }

  add_for_server(msg::prepare_for_send(msg::message_received_t()));

  switch(type)
  {
    case msg::get_type<msg::is_server_lost_t>::value:
      break;
    case msg::get_type<msg::hello_server_t>::value:
      add_for_server(msg::prepare_for_send(msg::get_login_t()));
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
  helper::log("pull_for_server");

  const auto& _1 = messages_for_server.front();
  const std::string m = add_serial_num(_1.message, _1.is_extra ? send_serial_num : ++send_serial_num);

  if(msg::is_equal_types<msg::message_received_t>(_1.message))
    { begin_wait_receive(_1.message); }

  messages_for_server.erase(messages_for_server.begin());

  return m;
}

std::string client_t::impl_t::pull_for_logic()
{
  helper::log("pull_for_logic");

  const std::string m = messages_for_logic.front();
  messages_for_logic.erase(messages_for_logic.begin());

  return m;
}

endpoint_t client_t::impl_t::get_address() const
{
  return address;
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

  if(serial_num == received_serial_num - 1 && msg::is_equal_types<msg::message_received_t>(m))
  {
    start_connection_timer();
    add_for_server(msg::prepare_for_send(msg::message_received_t()), true);
    return false;
  }

  if(serial_num != received_serial_num)
  {
    helper::log("check_ser_num: Warning! Wrong serial number!");
    return false;
  }

  return true;
}

void client_t::impl_t::add_for_server(const std::string& m, bool is_extra_message)
{
  helper::log("add_for_server: ", m);
  auto _1 = server_mess_t(m, is_extra_message);
  if(is_extra_message)
    { messages_for_server.insert(messages_for_server.begin(), _1); }
  else
    { messages_for_server.push_back(_1); }
}

void client_t::impl_t::begin_wait_receive(const std::string& message)
{
  is_received = false;
  last_send_message = message;
  start_response_timer();
}

void client_t::impl_t::is_message_received()
{
  helper::log("is_message_received");
  static int num_of_restarts = 0;
  if(is_received)
  {
    response_timer.cancel();
    num_of_restarts = 0;
  }
  else
  {
    add_for_server(last_send_message, true);

    if(msg::is_equal_types<msg::is_client_lost_t>(last_send_message) || num_of_restarts == 3)
      { messages_for_logic.push_back(msg::prepare_for_send(msg::client_lost_t())); }

    ++num_of_restarts;
    start_response_timer();
  }
}

void client_t::impl_t::connection_timer_timeout()
{
  add_for_server(msg::prepare_for_send(msg::is_client_lost_t()));
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

void client_t::impl_t::start_connection_timer()
{
  helper::log("start_connection_timer();");
  connection_timer.cancel();
  connection_timer.expires_from_now(boost::posix_time::milliseconds(7000));
  connection_timer.async_wait([&](const boost::system::error_code& error){ if(!error){connection_timer_timeout();} helper::log(">>>>connection_timer<<<<", (error ? " error!" : ""));});
//  connection_timer.async_wait([&](auto /*e*/){sr::log(">>>>connection_timer<<<<");});
}

void client_t::impl_t::start_response_timer()
{
  helper::log("start_response_timer();");
  response_timer.cancel();
  response_timer.expires_from_now(boost::posix_time::milliseconds(1500));
  response_timer.async_wait( [&](const boost::system::error_code& error)
  { if(!error){ is_message_received(); } helper::log(">>>>response_timer<<<<", (error ? " error!" : ""));});
  //response_timer.async_wait([&](auto /*e*/){sr::log(">>>>response_timer<<<<");});
}

client_t::impl_t::server_mess_t::server_mess_t(const std::string& m, const bool is_extra_message)
    : message(m), is_extra(is_extra_message)
{
}
