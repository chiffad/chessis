#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include "headers/integration.h"
#include "headers/chess.h"

Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : m_name(name), m_x(x), m_y(y), m_visible(visible)
{
}

ChessIntegration::ChessIntegration(QObject *parent)
    : QAbstractListModel(parent)
{
  board = new Board();
}

 QString ChessIntegration::move_turn_color() const
 {
   return m_move_color;
 }

 QString ChessIntegration::coord_in_simbols() const
 {
   return m_convert_coord_in_letter;
 }

void ChessIntegration::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_figures_model << figure;
  endInsertRows();
}

int ChessIntegration::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_figures_model.count();
}

QVariant ChessIntegration::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= m_figures_model.count())
    return QVariant();

  const Figure &figure = m_figures_model[index.row()];
  if (role == NameRole)
    return figure.name();
  else if (role == XRole)
    return figure.x();
  else if (role == YRole)
    return figure.y();
  else if (role == VisibleRole)
    return figure.visible();
  return QVariant();
}

QHash<int, QByteArray> ChessIntegration::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}

int ChessIntegration::get_index(const Board::Coord& coord) const
{
  int index;
  for(index = 0; index < rowCount(); ++index)
  {
    if(m_figures_model[index].x() == coord.x && m_figures_model[index].y() == coord.y)
      break;
  }
  return index;
}

unsigned int correct_figure_coord(const unsigned int coord)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  return (coord + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::move(unsigned int x, unsigned int y)
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

    if(board->move(board->from, board->to))
    {
      switch_move_color();

      m_convert_coord_in_letter = letter_return(board->to.x +1) + (board->to.y + 1);
      emit coord_in_simbols_changed();

      set_new_figure_coord(board->from, board->to);
    }
    else set_new_figure_coord(board->from, board->from);
  }
}

void ChessIntegration::switch_move_color()
{
  if(board->get_prev_color() == W_FIG)
    m_move_color = "white";
  else  m_move_color = "black";

  emit move_turn_color_changed();
}

void ChessIntegration::set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord)
{
  const int INDEX;
  if(board->get_field(new_coord) != FREE_FIELD && old_coord.x != new_coord.x && old_coord.y != new_coord.y)
  {
    INDEX = get_index(new_coord);
    m_figures_model[INDEX].set_visible(false);
    emit_data_change(INDEX);
  }

  INDEX = get_index(old_coord);
  if(INDEX < rowCount())
  {
    m_figures_model[INDEX].set_coord(new_coord);
    emit_data_change(INDEX);
  }
  else qDebug()<<"index out of range";
}

void ChessIntegration::emit_data_change(const int INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

void ChessIntegration::back_move()
{
  qDebug()<<"back move";
  board->back_move();
}

QString ChessIntegration::letter_return(const int number)
{
  int letter = 'a';
  letter += number;
  return QChar(letter);
}









