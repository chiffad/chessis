#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include "headers/integration.h"
#include "headers/chess.h"

Figure::Figure(const QString &name, const int x, const int y, const bool visible)
    : m_name(name), m_x(x), m_y(y), m_visible(visible)
{
}

CppIntegration::CppIntegration(QObject *parent)
    : QAbstractListModel(parent)
{
  board = new Board();
}

void CppIntegration::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_figure << figure;
  endInsertRows();

  //qDebug()<<m_figure[0].name();
  //m_figure.at(0).set_name("woo");
  // m_figure.at(0).value(int i)
  //m_figure[0].set_name("w_e");
  //qDebug()<<m_figure.count();
}

int CppIntegration::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_figure.count();
}

QVariant CppIntegration::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= m_figure.count())
    return QVariant();

  const Figure &figure = m_figure[index.row()];
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

QHash<int, QByteArray> CppIntegration::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}

const unsigned int correct_figure_coord(const unsigned int coord)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  return (coord + IMG_MID) / CELL_SIZE;
}

void CppIntegration::move(unsigned int x, unsigned int y)
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
      if(board->get_prev_color() == W_FIG)
        move_color = "white";

      else  move_color = "black";

      convert_coord_in_letter = letter_return(board->to.x +1) + (board->to.y + 1);
      emit convert_move_coord_in_letter_changed();

      emit move_turn_color_changed();

      //m_figure[0].set_coord(board->to);
    }
    //else m_figure[0].set_coord(board->from);

    qDebug()<<"here";//m_figure.count();
  }
}

void CppIntegration::back_move()
{
  qDebug()<<"back move";
  board->back_move();
}

const QString CppIntegration::letter_return(const int number)
{
  int letter = 'a';
  letter += number;
  return QChar(letter);
}








