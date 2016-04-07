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
  m_move_color = "img/w_k.png";
}

void ChessIntegration::move(const unsigned x, const unsigned y)
{
  static bool is_from = true;
  //if(!is_check_mate())
  //{
    if(is_from)
    {
      correct_figure_coord(board->from,x,y);
      if(board->get_field(board->from) != FREE_FIELD)
        is_from = false;
    }

    else
    {
      is_from = true;
      correct_figure_coord(board->to, x, y);
      if(board->move(board->from, board->to))
      {
        //switch_move_color(); // work!
        //add_to_history(board->from, board->to);
        set_new_figure_coord(board->from, board->to);
      }
      else set_new_figure_coord(board->from, board->from);
    }
  //}
  //else emit check_mate();
}

void ChessIntegration::back_move()
{
  qDebug()<<"back move";
  const Board::Coord last_from_coord = board->prev_from_coord();
  const Board::Coord last_to_coord = board->prev_to_coord();
  qDebug()<<last_from_coord.x<<" "<<last_from_coord.y;
  if(board->back_move())
    set_new_figure_coord(last_to_coord, last_from_coord, true);
}

int ChessIntegration::get_index(const Board::Coord& coord, int index) const
{
  for(; index < rowCount(); ++index)
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
  qDebug()<<board->is_figure_was_beaten_in_last_move();

  if(!back_move && board->is_figure_was_beaten_in_last_move() && old_coord.x != new_coord.x && old_coord.y != new_coord.y)
  {
    int i = 0;
    for(; i < rowCount(); ++i )
      if(m_figures_model[i].visible())
      {
        qDebug()<<"here1"<<i;
        index = get_index(new_coord, i);
      }

    qDebug()<<"here2"<<i;
    //if(i >= rowCount())
      //index = get_index(new_coord);

    m_figures_model[index].set_visible(false);
    qDebug()<<"here"<<i;
    emit_data_changed(index);
  }

  index = get_index(old_coord);
  if(index < rowCount())
  {
    m_figures_model[index].set_coord(new_coord);
    emit_data_changed(index);
  }
  else qDebug()<<"out of range";

  if(back_move)
  {
    index = get_index(old_coord);
    qDebug()<<index;
    if(index < rowCount())
    {
      m_figures_model[index].set_visible(true);
      emit_data_changed(index);
    }
  }

}

/*void ChessIntegration::switch_move_color() // work!
{
  if(board->get_prev_color() == W_FIG)
    m_move_color = "img/w_k.png";
  else  m_move_color = "img/b_K.png";

  emit move_turn_color_changed();
}

QString ChessIntegration::move_turn_color() const // work!
{
  return m_move_color;
}*/

void ChessIntegration::emit_data_changed(const int INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool ChessIntegration::is_check_mate() const//!!!!!!!!
{
  if(board->get_current_move() < 2) return false;
  return board->is_mate(board->get_color(board->prev_to_coord()));
}

/*QStringList ChessIntegration::moves_history() const
{
  return m_moves_history;
}*/

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


