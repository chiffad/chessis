#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <QMouseEvent>
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

void ChessIntegration::mouse_event(const QMouseEvent* event)
{
  if(event == Qt::LeftButton)
    move(event->x(), event->y());

  else if(Qt::RightButton)
    back_move();
}

void ChessIntegration::move(unsigned int x, unsigned int y)
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
      switch_move_color();
      //add_to_moves_history(board->from, board->to);
      set_new_figure_coord(board->from, board->to);
    }
    else set_new_figure_coord(board->from, board->from);
  }
}

void ChessIntegration::back_move()
{
  qDebug()<<"back move";
  const Board::Coord lase_from_coord = board->from;
  const Board::Coord lase_to_coord = board->to;
  board->back_move();
  set_new_figure_coord(lase_to_coord, lase_from_coord, true);
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

void ChessIntegration::correct_figure_coord(Board::Coord& coord, const unsigned int x, const unsigned int y)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  coord.x = (x + IMG_MID) / CELL_SIZE;
  coord.y = (y + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::set_new_figure_coord(const Board::Coord& old_coord, const Board::Coord& new_coord, bool back_move)
{
  const int INDEX;
  if(!back_move && board->get_field(new_coord) != FREE_FIELD && old_coord.x != new_coord.x && old_coord.y != new_coord.y)
  {
    INDEX = get_index(new_coord);
    m_figures_model[INDEX].set_visible(false);
    emit_data_change(INDEX);
  }

  else if(board->get_field(old_coord) != FREE_FIELD)
  {
    INDEX = get_index(old_coord);
    m_figures_model[INDEX].set_visible(true);
    emit_data_change(INDEX);
  }

  INDEX = get_index(old_coord);
  if(INDEX < rowCount())
  {
    m_figures_model[INDEX].set_coord(new_coord);
    emit_data_change(INDEX);
  }
}

/*void ChessIntegration::add_to_moves_history(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  const int a_LETTER = 'a';
  m_history_move = QChar(a_LETTER + coord_from.x) + QString::setNum(coord_from.y) + '-' + QChar(a_LETTER + coord_to.x) + QString::setNum(coord_to.y);
  emit history_move_changed();
}*/

void ChessIntegration::switch_move_color()
{
  if(board->get_prev_color() == W_FIG)
    m_move_color = "white";
  else  m_move_color = "black";

  emit move_turn_color_changed();
}

void ChessIntegration::emit_data_change(const int INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

QString ChessIntegration::move_turn_color() const
{
  return m_move_color;
}

QString ChessIntegration::history_move() const
{
  return m_history_move;
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
