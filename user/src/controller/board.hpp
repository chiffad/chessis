#pragma once

#include "controller/figures_model.hpp"
#include "coord.h"
#include "messages/messages.hpp"

#include <QQmlApplicationEngine>
#include <functional>

namespace controller {

class board_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)
  Q_PROPERTY(bool playing_white READ playing_white NOTIFY playing_white_changed)

public:
  using move_requested_callback_t = std::function<void(const std::string& move_msg)>;

public:
  explicit board_t(const move_requested_callback_t& callback);
  board_t(const board_t&) = delete;
  board_t& operator=(const board_t&) = delete;
  ~board_t() override;

  void set_context_properties(QQmlApplicationEngine& engine);
  void update_game_info(const msg::game_inf_t& game_info);
  bool playing_white() const;
  bool is_check_mate() const;

  Q_INVOKABLE void move(int x1, int y1, int x2, int y2);
  Q_INVOKABLE void set_cell_size(int width, int height);

signals:
  void check_mate();
  void playing_white_changed();

private:
  void set_playing_white(bool playing_white);
  void set_check_mate();
  void update_hilight(int move_num, const std::string& history);
  void set_board_mask(const std::string& mask);
  void update_figures();
  coord_t get_coord(int x, int y) const;
  coord_t according_to_side(const coord_t& c) const;

private:
  figures_model_t figures_model_;
  bool playing_white_;
  bool check_mate_;
  std::string field_;
  unsigned cell_width_;
  unsigned cell_height_;
  move_requested_callback_t move_requested_callback_;
};

} // namespace controller
