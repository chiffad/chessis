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
      if(board->get_figure(board->from) != FREE_FIELD)
      {
       // m_figures_model[FIRST_HILIGHT].set_visible(true);
       // m_figures_model[FIRST_HILIGHT].set_coord(board->from);
       // m_figures_model[SECOND_HILIGHT].set_visible(false);

        is_from = false;
      }
    }

    else
    {
      is_from = true;
      correct_figure_coord(board->to, x, y);
      if(board->move(board->from, board->to))
      {
        //m_figures_model[SECOND_HILIGHT].set_visible(true);
        //m_figures_model[SECOND_HILIGHT].set_coord(board->to);
        //switch_move_color(); // work!
        //add_to_history(board->from, board->to);//!!!

      }
      update_coordinates();
    }
  //}
  //else emit check_mate();
}

void ChessIntegration::back_move()
{
  qDebug()<<"back move";
  if(board->back_move())
  {
   // m_figures_model[FIRST_HILIGHT].set_visible(false);
    //m_figures_model[SECOND_HILIGHT].set_visible(false);
    update_coordinates();
  }
}

int ChessIntegration::get_index(const Board::Coord& coord, int index) const
{
  for(; index < rowCount() ; ++index)
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

void ChessIntegration::update_coordinates()
{
  int index = 0;
  Board::Coord a;

  for(; index < rowCount(); ++index)
  {
    a.x = m_figures_model[index].x();
    a.y = m_figures_model[index].y();
    if(board->get_figure(a) == FREE_FIELD)
    {
      m_figures_model[index].set_visible(false);
      emit_data_changed(index);
    }
  }

  index = 0;
  Board::Coord coord;
  for(coord.y = 0; coord.y < Y_SIZE; ++coord.y)
    for(coord.x = 0; coord.x < X_SIZE; ++coord.x)
      if(board->get_figure(coord) != FREE_FIELD)
      {
        QString fig_name_color;
        if(board->get_color(coord) == W_FIG)
          fig_name_color = "b_";
        else fig_name_color = "w_";
        fig_name_color += board->get_figure(coord);

        m_figures_model[index].set_name(fig_name_color);
        m_figures_model[index].set_coord(coord);
        m_figures_model[index].set_visible(true);

        emit_data_changed(index);
        ++index;
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

/*bool ChessIntegration::is_check_mate() const//!!!!!!!!
{
  if(board->get_current_move() < 2) return false;
  return board->is_mate(board->get_color(board->prev_to_coord()));
}*/

/*QStringList ChessIntegration::moves_history() const //!!!!
{
  return m_moves_history;
}*/

/*void ChessIntegration::add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to) //!!!!
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
