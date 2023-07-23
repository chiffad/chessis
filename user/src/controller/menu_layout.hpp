#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <functional>
#include <vector>

#include "client/messages.h"

namespace controller {

class menu_layout_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString move_turn_color READ move_turn_color NOTIFY move_turn_color_changed)
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

  void set_move_color(const int move_num);
  void set_board_mask(const QString& mask);
  void set_connect_status(const int status);
  void set_moves_history(const QString& history);
  void add_to_command_history(const QString& str);

  QString move_turn_color() const;
  QStringList moves_history() const;
  QStringList commands_hist() const;
  int last_command_hist_ind() const;
  QString connection_status() const;

  Q_INVOKABLE void path_to_file(QString& path, bool is_moves_from_file);
  Q_INVOKABLE void run_command(const QString& message, const int x);

signals:
  void moves_history_changed();
  void commands_hist_changed();
  void move_turn_color_changed();
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
  QString move_color_;
  QString connection_status_;
  QStringList str_moves_history_;
  QStringList commands_history_;
  QString field_;

  command_requested_callback_t command_requested_callback_;
};

} // namespace controller