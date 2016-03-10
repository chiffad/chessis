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

void Integr::back_move()
{
  qDebug()<<"back_move!";
  //board->back_move();
}

void Integr::set_visible(bool visible)
{
  figure_visible = visible;
  emit visible_changed();
}

unsigned int Integr::correct_img_coord(unsigned int coord)
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
    if(char(board->get_field(board->to)) != FREE_FIELD && board->move(board->from, board->to))
    {
      Integr.set_visible(faalse);
      return true;
    }
  }
  return false;
}

bool Integr::is_free_field(unsigned int x, unsigned int y)
{
  coord.x = x;
  coord.y = y;
  if(board->get_field(coord) != FREE_FIELD)
    return false;

  return true;
}

/*QChar correct_figure(QString fig)
{
  return fig[fig.length() - 1];
}

bool Integr::is_not_beaten(unsigned int x, unsigned int y, QString fig)
{
  coord.x = x;;
  coord.y = y;
  if(char(board->get_field(coord)) != correct_figure(fig))
    return false;

  else return true;
}*/













