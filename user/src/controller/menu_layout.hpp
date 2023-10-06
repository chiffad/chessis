#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <functional>
#include <vector>

#include <spdlog/spdlog.h>

namespace controller {

class menu_layout_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool white_move_turn READ white_move_turn NOTIFY white_move_turn_changed)
  Q_PROPERTY(QStringList moves_history READ moves_history NOTIFY moves_history_changed)
  Q_PROPERTY(QStringList commands_hist READ commands_hist NOTIFY commands_hist_changed)
  Q_PROPERTY(int last_command_hist_ind READ last_command_hist_ind NOTIFY commands_hist_changed)
  Q_PROPERTY(QString connection_status READ connection_status NOTIFY connection_status_changed)

public:
  struct command_requested_callbacks_t
  {
    std::function<void(std::string)> move;
    std::function<void(uint16_t)> go_to_history;
    std::function<void()> new_game;
    std::function<void()> back_move;
    std::function<void()> opponent_inf;
    std::function<void()> my_inf;
  };

  enum class connection_status_t
  {
    server_available,
    server_lost,
    opponent_lost
  };

public:
  explicit menu_layout_t(const command_requested_callbacks_t& callbacks);
  menu_layout_t(const menu_layout_t&) = delete;
  menu_layout_t& operator=(const menu_layout_t&) = delete;
  ~menu_layout_t();

  void update_game_info(int move_num, const std::string& moves_history);
  void set_connect_status(connection_status_t status);
  void add_to_command_history(const QString& str);

  bool white_move_turn() const;
  QStringList moves_history() const;
  QStringList commands_hist() const;
  int last_command_hist_ind() const;
  QString connection_status() const;

  Q_INVOKABLE void path_to_file(QString& path, bool is_moves_from_file);
  Q_INVOKABLE void run_command(const QString& message);
  Q_INVOKABLE void new_game();
  Q_INVOKABLE void back_move();
  Q_INVOKABLE void go_to_history(int hist_i);

signals:
  void moves_history_changed();
  void commands_hist_changed();
  void white_move_turn_changed();
  void connection_status_changed();

private:
  void set_moves_history(const QString& history);
  void set_move_turn(int move_num);

  void write_moves_to_file(const QString& path);
  void read_moves_from_file(const QString& path);

private:
  bool white_move_turn_;
  QString connection_status_;
  QStringList str_moves_history_;
  QStringList commands_history_;
  QString field_;
  command_requested_callbacks_t command_requested_;
};

} // namespace controller
