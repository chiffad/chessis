#include "headers/integration.h"
#include "headers/chess.h"
#include <QPainter>
#include <QString>

Integr::Integr(QObject *parent)
    : QObject(parent)
{
  board = new Board();
}

void Integr::back_move()
{
  qDebug()<<"back_move!";
  //board->back_move();
}

void Integr::set_coord_x(const int& val)
{
  coord.x = val;
}

void Integr::set_coord_y(const int& val)
{
  coord.y = val;
}

unsigned int Integr::correct_img_coord(unsigned int coord)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  return int((coord + IMG_MID) / CELL_SIZE) * CELL_SIZE;
}

bool Integr::move(int x, int y)
{
  static bool is_from = true;
  if(is_from)
  {
    board->from.x = x;
    board->from.y = y;
    if(char(board->get_field(board->from)) != FREE_FIELD)
      is_from = false;
  }

  else
  {
    is_from = true;
    board->to.x = x;
    board->to.y = y;
    qDebug()<<"from: "<<board->from.x<<""<<board->from.y;
    qDebug()<<"to: "<<board->to.x<<""<<board->to.y;
    qDebug()<<"fig: "<<char(board->get_field(board->from));
    qDebug()<<"fig: "<<char(board->get_field(board->to));

    return board->move(board->from, board->to);
  }
  return false;
}
