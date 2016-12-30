#ifndef __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL
#define __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

#include <string>
#include <memory>
#include <boost/asio.hpp>


namespace sr
{

class client_t
{
public:
  client_t(boost::asio::io_service& io_serv, const boost::asio::ip::udp::endpoint& addr);
  ~client_t();
  void push_from_server(const std::string& message);
  void push_for_send(const std::string& message);
  bool is_message_for_server_append() const;
  bool is_message_for_logic_append() const;
  std::string pull_for_server();
  std::string pull_for_logic();
  boost::asio::ip::udp::endpoint get_address() const;

  void set_login(const std::string& log);
  std::string get_login() const;
  void set_rating(const int rating);
  int get_rating() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;


private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

