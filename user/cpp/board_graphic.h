#ifndef __MY_BOARD_GRAPHIC_H__SDAWBNUIBMGYUIVGFKEYUVFYUFAPIECHARTH
#define __MY_BOARD_GRAPHIC_H__SDAWBNUIBMGYUIVGFKEYUVFYUFAPIECHARTH


#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QVector>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include <fstream>

#include "figure.h"
#include "coord.h"
#include "enums.h"


namespace graphic
{

class Board_graphic : public QAbstractListModel
{
  Q_OBJECT

public:
  Board_graphic();
  ~Board_graphic();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;

public:
  Q_PROPERTY(QString get_move_turn_color READ get_move_turn_color NOTIFY move_turn_color_changed)
  QString get_move_turn_color() const;

  Q_PROPERTY(QStringList get_moves_history READ get_moves_history NOTIFY moves_history_changed)
  QStringList get_moves_history() const;

  Q_PROPERTY(QStringList get_commands_hist READ get_commands_hist NOTIFY commands_hist_changed)
  QStringList get_commands_hist() const;

  Q_PROPERTY(int get_last_command_hist_ind READ get_last_command_hist_ind NOTIFY commands_hist_changed)
  int get_last_command_hist_ind() const;

  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)
  bool is_check_mate() const;

  Q_PROPERTY(QString get_udp_connection_status READ get_udp_connection_status NOTIFY udp_connection_status_changed)
  QString get_udp_connection_status() const;

  Q_INVOKABLE void run_command(const QString &message,  const int x1 = 0, const int y1 = 0,
                               const int x2 = 0, const int y2 = 0);
  Q_INVOKABLE void path_to_file(QString &path, bool is_moves_from_file);
  Q_INVOKABLE bool set_login(const QString &login);

public:
  void set_check_mate();
  void set_move_color(const int move_num);
  void set_board_mask(const QString &mask);
  void set_connect_status(const int status);
  bool is_message_appear() const;
  const QString pull();
  void set_moves_history(const QString &history);
  void add_to_command_history(const QString& str);
  void update_hilight(const int move_num, const QString& history);

signals:
  void check_mate();
  void moves_history_changed();
  void commands_hist_changed();
  void move_turn_color_changed();
  void udp_connection_status_changed();

public:
  Board_graphic(const Board_graphic&) = delete;
  Board_graphic& operator=(const Board_graphic&) = delete;

private:
  enum FigureRoles {
      NameRole = Qt::UserRole + 3,
      XRole = Qt::UserRole + 2,
      YRole = Qt::UserRole + 1,
      VisibleRole = Qt::UserRole
  };
  enum { CELL_NUM = 8, CELL_SIZE_X = 56, CELL_SIZE_Y = 36, a_LETTER = 'a'};
  const QString MOVE_COLOR_W = "img/w_k.png";
  const QString MOVE_COLOR_B = "img/b_K.png";
  const char FREE_SPACE = ' ';

private:
  void addFigure(const Figure &figure);
  void update_coordinates();
  Coord get_field_coord(const int i) const;
  void write_moves_to_file(const QString& path);
  void emit_figure_changed(const unsigned index);
  void read_moves_from_file(const QString& path);
  const QString coord_to_str(const Coord& from, const Coord& to) const;
  bool set_correct_coord(Coord& coord, const int x, const int y);
  void add_to_messages_for_server(const Messages::MESSAGE mes_type, const QString& content = QString());

private:
  QString _move_color;
  QString _udp_connection_status;
  bool _is_check_mate;
  QStringList _str_moves_history;
  QStringList _commands_history;
  QList<Figure> _figures_model;
  QVector<QString> _messages_for_server;
  QString _field;
};

} // namespace graphic

#endif // __MY_BOARD_GRAPHIC_H__SDAWBNUIBMGYUIVGFKEYUVFYUFAPIECHARTH