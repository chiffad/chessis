#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QString>
#include <QByteArray>
#include "headers/board_graphic.h"
#include "headers/udp_socet.h"

class Exporter
{
public:
  explicit Exporter(Board_graphic *const board_graphic, UDP_socet *const udp_socet);
  ~Exporter(){}

public:
  void push_to_graphic(const QString &message);
  void push_to_socet(const QString &message);
  const QString pull_from_graphic();
  const QString pull_from_socet();

private:
  Board_graphic *const _board_graphic;
  UDP_socet *const _udp_socet;
};
#endif // UDP_CLIENT_H
