#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <functional>
#include <vector>

#include <messages/messages.hpp>

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
  using command_requested_callback_t = std::function<void(std::string)>;

public:
  explicit menu_layout_t(const command_requested_callback_t& callback);
  menu_layout_t(const menu_layout_t&) = delete;
  menu_layout_t& operator=(const menu_layout_t&) = delete;
  ~menu_layout_t();

  void set_move_turn(int move_num);
  void set_board_mask(const QString& mask);
  void set_connect_status(int status);
  void set_moves_history(const QString& history);
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
  void write_moves_to_file(const QString& path);
  void read_moves_from_file(const QString& path);

  template<typename T>
  inline void command_requested(T&& command)
  {
    command_requested_callback_(msg::prepare_for_send(std::forward<T>(command)));
  }

private:
  bool white_move_turn_;
  QString connection_status_;
  QStringList str_moves_history_;
  QStringList commands_history_;
  QString field_;
  command_requested_callback_t command_requested_callback_;
};

} // namespace controller
