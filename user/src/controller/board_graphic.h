#pragma once

#include "client/messages.h"
#include "controller/figures_model.hpp"
#include "coord.h"

#include <QQmlApplicationEngine>
#include <QString>
#include <QStringList>
#include <functional>

namespace controller {

class board_graphic_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)

public:
  using move_requested_callback_t = std::function<void(msg::move_t)>;

public:
  explicit board_graphic_t(const move_requested_callback_t& callback);
  board_graphic_t(const board_graphic_t&) = delete;
  board_graphic_t& operator=(const board_graphic_t&) = delete;
  ~board_graphic_t() override;

  bool is_check_mate() const;
  void set_check_mate();
  void update_hilight(int move_num, const QString& history);
  void set_board_mask(const QString& mask);
  void set_context_properties(QQmlApplicationEngine& engine);

  Q_INVOKABLE void move(int x1, int y1, int x2, int y2);
  Q_INVOKABLE void set_cell_size(int width, int height);

signals:
  void check_mate();

private:
  void update_figures();
  coord_t get_coord(int x, int y) const;

private:
  figures_model_t figures_model_;
  bool check_mate_;
  QString field_;
  unsigned cell_width_;
  unsigned cell_height_;
  move_requested_callback_t move_requested_callback_;
};

} // namespace controller
