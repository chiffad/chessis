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

/*void Integr::back_move()
{
  qDebug()<<"back_move!";
  //board->back_move();
}

void Integr::set_visible(bool visible)
{
  figure_visible = visible;
  emit visible_changed();
}*/

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
    qDebug()<<"from: "<<board->from.x<<""<<board->from.y;
    qDebug()<<"to: "<<board->to.x<<""<<board->to.y;
    qDebug()<<"fig: "<<char(board->get_field(board->from));
    qDebug()<<"fig: "<<char(board->get_field(board->to));

    return  board->move(board->from, board->to);
  }
  return false;
}

bool Integr::is_free_field(unsigned int x, unsigned int y)
{
  coord.x = correct_figure_coord(x);
  coord.y = correct_figure_coord(y);
  if(board->get_field(coord) != FREE_FIELD)
    return false;

  return true;
}

bool Integr::is_the_same_coord(unsigned int x_candidate, unsigned int y_candidate,
                               unsigned int x, unsigned int y)
{
  if(x_candidate == correct_figure_coord(x) && y_candidate == correct_figure_coord(y))
    return true;

  else return false;
}








