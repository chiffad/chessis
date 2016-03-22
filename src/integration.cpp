#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include "headers/integration.h"
#include "headers/chess.h"

Integr::Integr(QObject *parent)
    : QObject(parent)
{
  board = new Board();
}

void Integr::set_move_turn_color(QString color)
{
  if(color == "white")
    move_color = "img/w_k.png";
  else move_color = "img/K.png";

  emit move_turn_color_changed();
}

void Integr::set_w_move_in_letter(QString c)
{
  const int a_LETTER = 97;
  QChar letter = a_LETTER + board->to.x + 1;
  w_move_letter = QString(letter) + QString(board->to.y + 1);

  emit w_move_in_letter_changed();
}

void Integr::set_b_move_in_letter(QString c)
{
  const int a_LETTER = 97;
  QChar letter = a_LETTER + board->to.x + 1;
  b_move_letter = QString(letter) + QString(board->to.y + 1);

  emit b_move_in_letter_changed();
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
    if(board->move(board->from, board->to))
    {

      return true;
    }
  }
  return false;
}

void Integr::back_move()
{
  qDebug()<<"back move";
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

const int Integr::prev_to_coord(QString selected_coord)
{
  Board::Coord to_coord = board->prev_to_coord();
  if(selected_coord == "y")
    return to_coord.y;
  else return to_coord.x;
}

const int Integr::prev_from_coord(QString selected_coord)
{
  Board::Coord from_coord = board->prev_from_coord();
  if(selected_coord == "y")
    return from_coord.y;
  else return from_coord.x;
}

QString Integr::figure_on_field_move_to()
{
  QChar figure_letter = board->figure_on_field_move_to();

  if(board->figure_on_field_move_to() == FREE_FIELD)
    return ".";

  if(board->figure_on_field_move_to() != toupper(board->figure_on_field_move_to()))
    return "w_" + QString(figure_letter);

  return figure_letter;
}

QChar Integr::letter_return(int number)
{
  int a = 'a';
  a += number;
  return a;
}

int Integr::see (int x)
{
    qDebug()<<x;

}







