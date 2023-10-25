#include "controller/board.hpp"

#include "figure.h"
#include "helper.h"
#include <spdlog/spdlog.h>

#include <QQmlContext>
#include <QString>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace chess::controller {

namespace {
const int CELL_NUM = 8;
const char FREE_FIELD = '.';
const char LETTER_a = 'a';
const char ASCII_0 = '0';
const size_t FIGURES_NUMBER = 32;

const size_t HILIGHT_CELLS_NUM = 2;
const size_t FIRST_HILIGHT_I = 32;
const size_t SECOND_HILIGHT_I = 33;
constexpr const char* const HILIGHT_IM = "hilight";

inline std::string to_str(const coord_t& c)
{
  return char(LETTER_a + c.x) + std::to_string(CELL_NUM - c.y);
}

inline coord_t get_field_coord(const int i)
{
  return coord_t(i % CELL_NUM, i / CELL_NUM);
}

QString figure_from_field_mask(const char figure_symbol)
{
  static const QString white_prefix = "w_";
  static const QString black_prefix = "b_";

  return (std::islower(figure_symbol) ? white_prefix : black_prefix) + figure_symbol;
}

} // namespace

board_t::board_t(const move_requested_callback_t& callback)
  : QObject(nullptr)
  , figures_model_{this}
  , playing_white_{true}
  , check_mate_{false}
  , field_{}
  , cell_width_{0}
  , cell_height_{0}
  , move_requested_callback_{callback}
{
  for (size_t i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS_NUM; ++i)
  {
    figures_model_.add_figure(figure_t{HILIGHT_IM, {0, 0}, false});
  }
}

board_t::~board_t() = default;

void board_t::set_context_properties(QQmlApplicationEngine& engine)
{
  engine.rootContext()->setContextProperty("FiguresModel", &figures_model_);
}

void board_t::update_game_info(const msg::game_inf_t& game_info)
{
  set_playing_white(game_info.playing_white);
  set_board_mask(game_info.board_mask);
  update_hilight(game_info.move_num, game_info.moves_history);

  if (game_info.is_mate)
  {
    set_check_mate();
  }
}

void board_t::move(const int x1, const int y1, const int x2, const int y2)
{
  SPDLOG_DEBUG("move requested: x1={}; y1={}; x1={}; y2={}", x1, y1, x2, y2);
  move_requested_callback_(to_str(get_coord(x1, y1)) + " - " + to_str(get_coord(x2, y2)));
}

void board_t::set_board_mask(const std::string& mask)
{
  if (mask.size() != CELL_NUM * CELL_NUM)
  {
    SPDLOG_ERROR("Wrong board mask size");
    return;
  }

  field_ = mask;
  update_figures();
}

void board_t::update_figures()
{
  std::string field_according_to_color = field_;
  if (!playing_white_) std::reverse(field_according_to_color.begin(), field_according_to_color.end());

  auto f_it = field_according_to_color.begin();
  for (size_t i = 0; i < FIGURES_NUMBER; ++i)
  {
    f_it = std::find_if(f_it, field_according_to_color.end(), [](auto const& i) { return i != FREE_FIELD; });
    figure_t f{"", {}, false};
    if (f_it != field_according_to_color.end())
    {
      f.set_coord(get_field_coord(std::distance(field_according_to_color.begin(), f_it)));
      f.set_name(figure_from_field_mask(*f_it));
      f.set_visible(true);
      ++f_it;
    }

    figures_model_.update_figure(f, i);
  }
}

void board_t::update_hilight(const int move_num, const std::string& history)
{
  if (move_num == 0)
  {
    figures_model_.update_figure(figure_t{HILIGHT_IM, {}, false}, FIRST_HILIGHT_I);
    figures_model_.update_figure(figure_t{HILIGHT_IM, {}, false}, SECOND_HILIGHT_I);
    return;
  }

  const int CHAR_IN_MOVE = 4;
  if (history.size() < move_num * CHAR_IN_MOVE) return;

  auto simb = history.begin() + (move_num - 1) * CHAR_IN_MOVE;
  coord_t c;
  for (size_t i = 0; i < HILIGHT_CELLS_NUM; ++i)
  {
    c.x = *(simb++) - LETTER_a;
    c.y = CELL_NUM - (*(simb++) - ASCII_0);
    figures_model_.update_figure(figure_t{HILIGHT_IM, according_to_side(c), true}, i == 0 ? SECOND_HILIGHT_I : FIRST_HILIGHT_I);
  }
}

bool board_t::is_check_mate() const
{
  return check_mate_;
}

void board_t::set_check_mate()
{
  check_mate_ = true;
  emit check_mate();
}

bool board_t::playing_white() const
{
  return playing_white_;
}

void board_t::set_playing_white(bool playing_white)
{
  playing_white_ = playing_white;
  emit playing_white_changed();
}

void board_t::set_cell_size(const int width, const int height)
{
  SPDLOG_INFO("Cell size: width={}; height={}", width, height);
  cell_width_ = width;
  cell_height_ = height;
}

coord_t board_t::get_coord(const int x, const int y) const
{
  if (x < 0 || y < 0 || x > (cell_width_ * CELL_NUM) || y > (cell_height_ * CELL_NUM))
  {
    SPDLOG_ERROR("Incorrect coord x={}; y={}", x, y);
    return coord_t(x, y);
  }

  const coord_t res{(x + (cell_width_ / 2)) / cell_width_, (y + (cell_height_ / 2)) / cell_height_};
  return according_to_side(res);
}

coord_t board_t::according_to_side(const coord_t& c) const
{
  return playing_white_ ? c : coord_t{CELL_NUM - 1 - c.x, CELL_NUM - 1 - c.y};
}

} // namespace chess::controller
