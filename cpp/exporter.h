#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QString>
#include <memory>
#include "board_graphic.h"
#include "udp_socket.h"

class Exporter
{
public:
  explicit Exporter(Board_graphic& board_graphic, UDP_socket& udp_socet);
  ~Exporter(){}

public:
  void push_to_graphic(const QString& message);
  void push_to_socet(const QString& message);
  const QString pull_from_graphic();
  const QString pull_from_socet();

public:
  Exporter(const Exporter&) = delete;
  Exporter& operator=(const Exporter&) = delete;

private:
  std::shared_ptr<Board_graphic> _board_graphic;
  std::shared_ptr<UDP_socket> _udp_socet;
};
#endif // UDP_CLIENT_H
