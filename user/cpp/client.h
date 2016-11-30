#ifndef __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL
#define __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

#include <QByteArray>
#include <memory>
#include "enums.h"


namespace inet
{

class client_t
{
public:
  client_t();
  ~client_t();
  void send(const QByteArray& message);
  QByteArray pull();
  bool is_message_append() const;

public:
  client_t(const client_t&) = delete;
  client_t& operator=(const client_t&) = delete;

private:
  struct impl_t;
  std::unique_ptr<impl_t> impl;
};

} // namespace inet

#endif // __CLIENT_H__GOVLIYVFTYUVFTYFDIKVGLUIHL

