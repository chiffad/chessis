#include "controller/board_graphic.h"

#include <QChar>
#include <QModelIndex>
#include <QPainter>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>

#include "helper.h"

namespace {
const int CELL_NUM = 8;
const char LETTER_a = 'a';
} // namespace

namespace controller {

board_graphic_t::board_graphic_t(const move_requested_callback_t& callback)
  : QAbstractListModel(nullptr)
  , check_mate_(false)
  , move_requested_callback_(callback)
{
  enum
  {
    HILIGHT_CELLS = 2,
    FIGURES_NUMBER = 32
  };
  const QString HILIGHT_IM = "hilight";

  for (int i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS; ++i)
  {
    addFigure(figure_t(HILIGHT_IM, 0, 0, false));
  }
}

board_graphic_t::~board_graphic_t() = default;

void board_graphic_t::move(const int x1, const int y1, const int x2, const int y2)
{
  SPDLOG_DEBUG("move requested: x1={}; y1={}; x1={}; y2={}", x1, y1, x2, y2);
  msg::move_t move_msg;
  move_msg.data = coord_to_str(get_coord(x1, y1), get_coord(x2, y2)).toStdString();
  move_requested_callback_(std::move(move_msg));
}

coord_t board_graphic_t::get_coord(const int x, const int y)
{
  if (x < 0 || y < 0 || x > (cell_width_ * CELL_NUM) || y > (cell_height_ * CELL_NUM))
  {
    SPDLOG_ERROR("Incorrect coord x={}; y={}", x, y);
    return coord_t(x, y);
  }

  return coord_t((x + (cell_width_ / 2)) / cell_width_, (y + (cell_height_ / 2)) / cell_height_);
}

void board_graphic_t::update_coordinates()
{
  const auto FREE_FIELD = '.';

  auto f_it = field_.begin();
  for (auto& fig_mod : figures_model_)
  {
    f_it = std::find_if(f_it, field_.end(), [&FREE_FIELD](auto const& i) { return i != FREE_FIELD; });
    if (f_it != field_.end())
    {
      fig_mod.set_coord(get_field_coord(field_.indexOf(*f_it, (f_it - field_.begin()))));
      fig_mod.set_name(QString(f_it->isLower() ? "w_" : "b_") + *f_it);
      fig_mod.set_visible(true);
      ++f_it;
    }
    else fig_mod.set_visible(false);
  }
}

coord_t board_graphic_t::get_field_coord(const int i) const
{
  return coord_t(i % CELL_NUM, i / CELL_NUM);
}

void board_graphic_t::set_board_mask(const QString& mask)
{
  if (mask.size() != CELL_NUM * CELL_NUM)
  {
    SPDLOG_ERROR("Wrong board mask size");
    return;
  }

  field_.clear();
  field_ = mask;

  update_coordinates();
}

const QString board_graphic_t::coord_to_str(const coord_t& from, const coord_t& to) const
{
  return (QChar(LETTER_a + from.x) + QString::number(CELL_NUM - from.y) + " - " + QChar(LETTER_a + to.x) + QString::number(CELL_NUM - to.y));
}

void board_graphic_t::update_hilight(const int move_num, const QString& history)
{
  enum HILIGHT
  {
    FIRST_HILIGHT = 32,
    SECOND_HILIGHT = 33
  };

  const int CHAR_IN_MOVE = 4;
  if (move_num && history.size() >= move_num * CHAR_IN_MOVE)
  {
    auto simb = history.begin();
    simb += (move_num - 1) * CHAR_IN_MOVE;
    coord_t c;
    for (int i = 0; i < 2; ++i)
    {
      c.x = (*(simb++)).unicode() - LETTER_a;
      c.y = CELL_NUM - (*(simb++)).digitValue();
      HILIGHT ind = (i == 0) ? SECOND_HILIGHT : FIRST_HILIGHT;
      figures_model_[ind].set_visible(true);
      figures_model_[ind].set_coord(c);
    }
  }
}

void board_graphic_t::redraw_board()
{
  QModelIndex topLeft = index(0, 0);
  QModelIndex bottomRight = index(figures_model_.size() - 1, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool board_graphic_t::is_check_mate() const
{
  return check_mate_;
}

void board_graphic_t::set_check_mate()
{
  check_mate_ = true;
  emit check_mate();
}

void board_graphic_t::set_cell_size(const int width, const int height)
{
  SPDLOG_INFO("Cell size: width={}; height={}", width, height);
  cell_width_ = width;
  cell_height_ = height;
}

void board_graphic_t::addFigure(const figure_t& figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  figures_model_ << figure;
  endInsertRows();
}

int board_graphic_t::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return figures_model_.count();
}

QVariant board_graphic_t::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= figures_model_.count())
  {
    return QVariant();
  }

  const figure_t& figure = figures_model_[index.row()];
  if (role == NameRole)
  {
    return figure.name();
  }
  else if (role == XRole)
  {
    return figure.x();
  }
  else if (role == YRole)
  {
    return figure.y();
  }
  else if (role == VisibleRole)
  {
    return figure.visible();
  }
  return QVariant();
}

QHash<int, QByteArray> board_graphic_t::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}

} // namespace controller
