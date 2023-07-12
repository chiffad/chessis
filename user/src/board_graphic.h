#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtQuick/QQuickPaintedItem>
#include <vector>

#include "coord.h"
#include "figure.h"
#include "messages.h"

namespace graphic {

class board_graphic_t : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(QString get_move_turn_color READ get_move_turn_color NOTIFY move_turn_color_changed)
  Q_PROPERTY(QStringList get_moves_history READ get_moves_history NOTIFY moves_history_changed)
  Q_PROPERTY(QStringList get_commands_hist READ get_commands_hist NOTIFY commands_hist_changed)
  Q_PROPERTY(int get_last_command_hist_ind READ get_last_command_hist_ind NOTIFY commands_hist_changed)
  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)
  Q_PROPERTY(QString get_udp_connection_status READ get_udp_connection_status NOTIFY udp_connection_status_changed)

public:
  board_graphic_t();
  ~board_graphic_t() override;

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;

public:
  QString get_move_turn_color() const;

  QStringList get_moves_history() const;

  QStringList get_commands_hist() const;

  int get_last_command_hist_ind() const;

  bool is_check_mate() const;

  QString get_udp_connection_status() const;

  Q_INVOKABLE void run_command(const QString& message, const int x1 = 0, const int y1 = 0, const int x2 = 0, const int y2 = 0);
  Q_INVOKABLE void path_to_file(QString& path, bool is_moves_from_file);
  Q_INVOKABLE bool set_login(const QString& login, const QString& pwd);
  Q_INVOKABLE void set_cell_size(int width, int height);

public:
  void set_check_mate();
  void set_move_color(const int move_num);
  void set_board_mask(const QString& mask);
  void set_connect_status(const int status);
  bool is_message_appear() const;
  const QString pull();
  void set_moves_history(const QString& history);
  void add_to_command_history(const QString& str);
  void update_hilight(const int move_num, const QString& history);
  void get_login(const QString& error_message = QString());
  void redraw_board();

signals:
  void check_mate();
  void moves_history_changed();
  void commands_hist_changed();
  void move_turn_color_changed();
  void udp_connection_status_changed();
  void enter_login(const QString& error_text);

public:
  board_graphic_t(const board_graphic_t&) = delete;
  board_graphic_t& operator=(const board_graphic_t&) = delete;

private:
  enum FigureRoles
  {
    NameRole = Qt::UserRole + 3,
    XRole = Qt::UserRole + 2,
    YRole = Qt::UserRole + 1,
    VisibleRole = Qt::UserRole
  };
  enum
  {
    CELL_NUM = 8,
    a_LETTER = 'a'
  };
  const QString MOVE_COLOR_W = "img/w_k.png";
  const QString MOVE_COLOR_B = "img/b_K.png";
  const char FREE_SPACE = ' ';

private:
  void addFigure(const figure_t& figure);
  void update_coordinates();
  Coord get_field_coord(const int i) const;
  void write_moves_to_file(const QString& path);
  void read_moves_from_file(const QString& path);
  const QString coord_to_str(const Coord& from, const Coord& to) const;
  Coord get_coord(const int x, const int y);
  void add_to_messages_for_server(const std::string& msg);

private:
  QString move_color_;
  QString udp_connection_status_;
  bool check_mate_;
  QStringList str_moves_history_;
  QStringList commands_history_;
  QList<figure_t> figures_model_;
  std::vector<QString> messages_for_server_;
  QString field_;

  unsigned cell_width_ = 0;
  unsigned cell_height_ = 0;
};

} // namespace graphic
