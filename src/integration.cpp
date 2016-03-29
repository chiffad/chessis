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
}

int CppIntegration::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_figure.count();
}

QVariant CppIntegration::data(const QModelIndex & index, int role) const {
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

QHash<int, QByteArray> CppIntegration::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "figure_name";
    roles[XRole] = "x_coord";
    roles[YRole] = "y_coord";
    roles[VisibleRole] = "figure_visible";
    return roles;
}

void CppIntegration::set_move_turn_color(QString color)
{
  if(color == "white")
    move_color = "img/w_k.png";
  else move_color = "img/K.png";

  emit move_turn_color_changed();
}

void CppIntegration::set_w_move_in_letter(QString c)
{
  const int a_LETTER = 97;
  QChar letter = a_LETTER + board->to.x + 1;
  w_move_letter = QString(letter) + QString(board->to.y + 1);

  emit w_move_in_letter_changed();
}

void CppIntegration::set_b_move_in_letter(QString c)
{
  const int a_LETTER = 97;
  QChar letter = a_LETTER + board->to.x + 1;
  b_move_letter = QString(letter) + QString(board->to.y + 1);

  emit b_move_in_letter_changed();
}

unsigned int CppIntegration::correct_figure_coord(unsigned int coord)
{
  const int CELL_SIZE = 560 / 8;
  const int IMG_MID = 40;
  return (coord + IMG_MID) / CELL_SIZE;
}

bool CppIntegration::move(unsigned int x, unsigned int y)
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
      if(board->get_prev_color() == W_FIG)
        set_move_turn_color("white");
      else set_move_turn_color("black");

      return true;
    }
  }
  return false;
}

void CppIntegration::back_move()
{
  qDebug()<<"back move";
  board->back_move();
}

bool CppIntegration::is_free_field(unsigned int x, unsigned int y)
{
  coord.x = correct_figure_coord(x);
  coord.y = correct_figure_coord(y);
  if(board->get_field(coord) != FREE_FIELD)
    return false;

  return true;
}

const int CppIntegration::prev_to_coord(QString selected_coord) const
{
  Board::Coord to_coord = board->prev_to_coord();
  if(selected_coord == "y")
    return to_coord.y;
  else return to_coord.x;
}

const int CppIntegration::prev_from_coord(QString selected_coord) const
{
  Board::Coord from_coord = board->prev_from_coord();
  if(selected_coord == "y")
    return from_coord.y;
  else return from_coord.x;
}

QString CppIntegration::figure_on_field_move_to()
{
  QChar figure_letter = board->figure_on_field_move_to();

  if(board->figure_on_field_move_to() == FREE_FIELD)
    return ".";

  if(board->figure_on_field_move_to() != toupper(board->figure_on_field_move_to()))
    return "w_" + QString(figure_letter);

  return figure_letter;
}

QChar CppIntegration::letter_return(int number)
{
  int a = 'a';
  a += number;
  return a;
}

int CppIntegration::see (int x)
{
    qDebug()<<x;

}







