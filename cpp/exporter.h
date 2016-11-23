#ifndef __MY_EXPORTER__HUIOGYUGFULBHOHUYFRTYDRT
#define __MY_EXPORTER__HUIOGYUGFULBHOHUYFRTYDRT

#include <QString>
#include <memory>

#include "board_graphic.h"
#include "my_socket.h"


class Exporter
{
public:
  Exporter(graphic::Board_graphic& board_graphic, inet::my_socket& socket);

public:
  void push_to_graphic(const QString& message);
  void push_to_socet(const QString& message);

public:
  Exporter(const Exporter&) = delete;
  Exporter& operator=(const Exporter&) = delete;

private:
  graphic::Board_graphic& _board_graphic;
  inet::my_socket& socket;
};


#endif // __MY_EXPORTER__HUIOGYUGFULBHOHUYFRTYDRT
