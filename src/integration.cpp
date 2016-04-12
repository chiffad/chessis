#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <vector>
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
  for(int i = 0; i < FIGURES_NUMBER; ++i)
  {
    addFigure(Figure("b_P", 0, 0, true));
  }
  addFigure(ChessIntegration::Figure("hilight", 0, 0, false));
  addFigure(ChessIntegration::Figure("hilight", 0, 0, false));
  update_coordinates();
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
        update_hilight(board->from, FIRST_HILIGHT, true);

        is_from = false;
      }
    }

    else
    {
      is_from = true;
      correct_figure_coord(board->to, x, y);
      if(!(board->from == board->to) && board->move(board->from, board->to))
      {
        update_hilight(board->to, SECOND_HILIGHT, true);
        //switch_move_color(); // work!
        add_to_history(board->from, board->to);//fix need
      }
      update_coordinates();
    }
  //}
  //else emit check_mate();
}


/*void ChessIntegration::move_to_history_index(const unsigned index)//!!
{
  if(index == board->get_current_move()) return;

  if(index < board->get_current_move())
  {
    const unsigned D_INDEX = board->get_current_move() - index;

    for(int i = 0; i < D_INDEX; ++i)
      back_move();
  }

  if(index > board->get_current_move())
  {}
}*/

/*void ChessIntegration::add_move_to_history_copy()//!!
{
  Copy_of_history_moves copy;

  copy.from = board->get_history_from_coord();
  copy.to = board->get_history_to_coord();

  history_copy.push_back(copy);
}*/

/*void ChessIntegration::back_move(bool true_back) //work!
{
  if(board->back_move())
  {
    update_hilight(board->get_history_from_coord(), FIRST_HILIGHT, true);
    update_hilight(board->get_history_to_coord(), SECOND_HILIGHT, true);

    update_coordinates();

    if(true_back)
      history_copy.pop_back();
  }
}*/

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
  for(; index < rowCount() - HILIGHT_CELLS; ++index)
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
          fig_name_color = "w_";
        else fig_name_color = "b_";
        fig_name_color += board->get_figure(coord);

        m_figures_model[index].set_coord(coord);
        m_figures_model[index].set_name(fig_name_color);
        m_figures_model[index].set_visible(true);

        emit_data_changed(index);
        ++index;
      }
}

void ChessIntegration::update_hilight(const Board::Coord& coord, HILIGHT hilight_index, bool visible)
{
  m_figures_model[hilight_index].set_visible(visible);
  m_figures_model[hilight_index].set_coord(coord);

  if(hilight_index == FIRST_HILIGHT)
  {
    m_figures_model[SECOND_HILIGHT].set_visible(false);
    emit_data_changed(SECOND_HILIGHT);
  }
  emit_data_changed(hilight_index);
}

/*void ChessIntegration::switch_move_color() // work!
{
  if(board->get_index_move_color_from_end(1) == W_FIG)
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

/*bool ChessIntegration::is_check_mate() const//need test
{
  return board->is_mate(board->get_index_move_color_from_end(2));
}*/

/*void ChessIntegration::start_new_game() //work!
{
  while (board->get_current_move() != 1)
    back_move();
}*/

QStringList ChessIntegration::moves_history() const //fix need
{
  return m_moves_history;
}

void ChessIntegration::add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to) //fix need
{
 // add_move_to_history_copy();

  unsigned correct_move = board->get_current_move() / 2;
  if(board->get_current_move() % 2 != 0) ++correct_move;

  const int a_LETTER = 'a';
  QString move;

  move.setNum(correct_move);

  if(board->get_color(coord_to) == B_FIG) move += "b ";
  else move += "w ";

  QString toY;
  QString fromY;

  move += QChar(a_LETTER + coord_from.x) + fromY.setNum(coord_from.y) + " - " + QChar(a_LETTER + coord_to.x) + toY.setNum(coord_to.y);

  m_moves_history.append(move);

  emit moves_history_changed();
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
