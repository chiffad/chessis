#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include "headers/integration.h"
#include "headers/chess.h"

ChessIntegration::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : m_name(name), m_x(x), m_y(y), m_visible(visible)
{
}

ChessIntegration::ChessIntegration(QObject *parent)
    : QAbstractListModel(parent)
{
  board = new Board();
}

void ChessIntegration::move(const unsigned x, const unsigned y)
{
  static bool is_from = true;
  if(is_from)
  {
    correct_figure_coord(board->from,x,y);
    if(char(board->get_field(board->from)) != FREE_FIELD)
      is_from = false;
  }

  else
  {
    is_from = true;
    correct_figure_coord(board->to, x, y);
    if(board->move(board->from, board->to))
    {
      //switch_move_color();
      //add_to_history(board->from, board->to);
      set_new_figure_coord(board->from, board->to);
    }
    else set_new_figure_coord(board->from, board->from);
  }
  if(is_check_mate()) emit check_mate();
}

/*void ChessIntegration::back_move()
{
  qDebug()<<"back move";
  const Board::Coord lase_from_coord = board->from;
  const Board::Coord lase_to_coord = board->to;
  board->back_move();
  set_new_figure_coord(lase_to_coord, lase_from_coord, true);
}*/

int ChessIntegration::get_index(const Board::Coord& coord) const
{
  int index;
  for(index = 0; index < rowCount(); ++index)
    if(m_figures_model[index].x() == coord.x && m_figures_model[index].y() == coord.y)
      break;

  return index;
}

void ChessIntegration::correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  coord.x = (x + IMG_MID) / CELL_SIZE;
  coord.y = (y + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord, bool back_move)
{
  int index;
  if(!back_move && board->get_field(new_coord) != FREE_FIELD && old_coord.x != new_coord.x && old_coord.y != new_coord.y)
  {
    index = get_index(new_coord);
    m_figures_model[index].set_visible(false);
    emit_data_changed(index);
  }

  if(back_move && board->get_field(old_coord) != FREE_FIELD)
  {
    index = get_index(old_coord);
    m_figures_model[index].set_visible(true);
    emit_data_changed(index);
  }

  index = get_index(old_coord);
  if(index < rowCount())
  {
    m_figures_model[index].set_coord(new_coord);
    emit_data_changed(index);
  }
  else qDebug()<<"out of range";
}

/*void ChessIntegration::switch_move_color()
{
  if(board->get_prev_color() == W_FIG)
    m_move_color = "white";
  else  m_move_color = "black";

  emit move_turn_color_changed();
}*/

void ChessIntegration::emit_data_changed(const int INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

/*QString ChessIntegration::move_turn_color() const
{
  return m_move_color;
}*/

/*QStringList ChessIntegration::moves_history() const
{
  return m_moves_history;
}*/

bool ChessIntegration::is_check_mate() const
{
  return board->is_mate(board->get_color(board->prev_to_coord()));
}

/*void ChessIntegration::add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  const int a_LETTER = 'a';
  QString move;
  if(m_move_color == "black") move.setNum(m_moves_history.count() / 2 + m_moves_history.count() % 2);
  move += QChar(a_LETTER + coord_from.x) + coord_from.y + '-' + QChar(a_LETTER + coord_to.x) + coord_to.y;

  m_moves_history.append(move);
  emit moves_history_changed();
}*/

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


