#include <QPainter>
#include <QString>
#include <QChar>
#include "headers/integration.h"
#include "headers/chess.h"

Integr::Integr(QObject *parent)
    : QObject(parent)
{
  board = new Board();
}

unsigned int Integr::correct_figure_coord(unsigned int coord)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  return (coord + IMG_MID) / CELL_SIZE;
}

bool Integr::move(unsigned int x, unsigned int y)
{
  static bool is_from = true;
  if(is_from)
  {
    board->from.x = correct_figure_coord(x);
    board->from.y = correct_figure_coord(y);
    if(char(board->get_field(board->from)) != FREE_FIELD)
      is_from = false;
  }

  else
  {
    is_from = true;
    board->to.x = correct_figure_coord(x);
    board->to.y = correct_figure_coord(y);

    if(board->from.x == board->to.x && board->from.y == board->to.y)
      return false;

    qDebug()<<"from: "<<board->from.x<<""<<board->from.y;
    qDebug()<<"to: "<<board->to.x<<""<<board->to.y;
    qDebug()<<"fig from: "<<char(board->get_field(board->from));
    qDebug()<<"fig to: "<<char(board->get_field(board->to));

    return  board->move(board->from, board->to);
  }
  return false;
}

void Integr::back_move()
{
  qDebug()<<"back_move!";

  emit back_move_signal_emmit();

  board->back_move();
}

bool Integr::is_free_field(unsigned int x, unsigned int y)
{
  coord.x = correct_figure_coord(x);
  coord.y = correct_figure_coord(y);
  if(board->get_field(coord) != FREE_FIELD)
    return false;

  return true;
}

const int Integr::prev_to_coord(QString selected_coord) const
{
  Board::Coord to_coord = board->prev_to_coord();
  if(selected_coord == "y")
    return to_coord.y;
  else return to_coord.x;
}


const int Integr::prev_from_coord(QString selected_coord) const
{
  Board::Coord from_coord = board->prev_from_coord();
  if(selected_coord == "y")
    return from_coord.y;
  else return from_coord.x;
}

int Integr::see (int x)
{
    qDebug()<<x;

}








