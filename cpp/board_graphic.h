#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include <vector>
#include <fstream>
#include <cpp/enums.h>
#include <QTimer>

class Board_graphic : public QAbstractListModel
{
  Q_OBJECT

public:
  enum HILIGHT {HILIGHT_CELLS = 2 , FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};
  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString BACK_MOVE = "back";
  const QString SHOW_ME = "show me";
  const QString NEW_GAME = "new game";
  const QString HISTORY = "to history";
  const QString SHOW_OPPONENT = "show opponent";

public slots:
  void timer_timeout();

private:
  class Figure;

public:
  enum FigureRoles {
      NameRole = Qt::UserRole + 3,
      XRole = Qt::UserRole + 2,
      YRole = Qt::UserRole + 1,
      VisibleRole = Qt::UserRole
  };
  enum {CELL_NUM = 8, FIGURES_NUMBER = 32};

public:
  explicit Board_graphic(QObject *parent = 0);
  ~Board_graphic();

  void addFigure(const Figure &figure);
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
  Q_INVOKABLE void set_login(const QString &login);

public:
  void set_check_mate();
  void set_move_color(const int move_num);
  void set_board_mask(const QString &mask);
  void set_connect_status(const int status);
  bool is_new_message_for_server_appear() const;
  const QString pull_first_messages_for_server();
  void set_moves_history(const QString &history);
  void update_hilight(const int move_num, const QString& history);

signals:
  void check_mate();
  void moves_history_changed();
  void commands_hist_changed();
  void move_turn_color_changed();
  void udp_connection_status_changed();

private:
  enum{ZERO_AND_ACTUAL_MOVES = 2, NEED_SIMB_TO_MOVE = 4, CELL_SIZE_X = 56, CELL_SIZE_Y = 36, a_LETTER = 'a', FREE_FIELD = '.'};
  const QString MOVE_COLOR_W = "img/w_k.png";
  const QString MOVE_COLOR_B = "img/b_K.png";
  const QString HILIGHT_IM = "hilight";
  const char FREE_SPACE = ' ';

private:
  struct Coord
  {
    int x;
    int y;
  };

private:
  Board_graphic(const Board_graphic&) = delete;
  Board_graphic& operator=(const Board_graphic&) = delete;

  void update_coordinates();
  Coord get_field_coord(const int i) const;
  void write_moves_to_file(const QString& path);
  void emit_figure_changed(const unsigned index);
  void read_moves_from_file(const QString& path);
  void add_to_command_history(const QString& str);
  const QString coord_to_str(const Coord& from, const Coord& to) const;
  bool set_correct_coord(Coord& coord, const int x, const int y);
  void add_to_messages_for_server_stack(const Messages::MESSAGE mes_type, const QString& content = QString());

private:
  QTimer *timer_kill;
  QString _move_color;
  QString _udp_connection_status;
  bool _is_check_mate;
  QStringList _str_moves_history;
  QStringList _commands_history;
  QList<Figure> _figures_model;
  QVector<QString> _messages_for_server_stack;
  QString _field;
};

class Board_graphic::Figure
{
public:
  Figure(const QString& name, const int x, const int y, const bool visible);
  ~Figure();

  inline int x() const {return _x;}
  inline int y() const {return _y;}
  inline QString name() const {return _name;}
  inline bool visible() const {return _visible;}

  inline void set_name(const QString& new_name) {_name = new_name;}
  inline void set_visible(const bool new_visible) {_visible = new_visible;}
  inline void set_coord(const Coord& new_coord) {_x = new_coord.x; _y = new_coord.y;}

  bool operator==(const Figure &rhs)
  {return(name() == rhs.name() && x() == rhs.x() && y() == rhs.y() && visible() == rhs.visible());}

private:
  QString _name;
  int _x;
  int _y;
  bool _visible;
};

#endif
