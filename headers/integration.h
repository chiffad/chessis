#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include <vector>
#include <fstream>
#include "headers/chess.h"
#include "headers/udp_client.h"

class ChessIntegration : public QAbstractListModel
{
  Q_OBJECT
private:
  class Figure;

public:
  enum FigureRoles {
      NameRole = Qt::UserRole + 3,
      XRole = Qt::UserRole + 2,
      YRole = Qt::UserRole + 1,
      VisibleRole = Qt::UserRole
  };

  explicit ChessIntegration(QObject *parent = 0);
  ~ChessIntegration(){delete _board; delete _udp_client;}

  void addFigure(const Figure &figure);
  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
  QHash<int, QByteArray> roleNames() const;

public:
  Q_PROPERTY(QString move_turn_color READ move_turn_color NOTIFY move_turn_color_changed)
  QString move_turn_color() const;

  Q_PROPERTY(QStringList moves_history READ moves_history NOTIFY moves_history_changed)
  QStringList moves_history() const;

  Q_PROPERTY(bool is_check_mate READ is_check_mate NOTIFY check_mate)
  bool is_check_mate() const;

  Q_INVOKABLE void move(const unsigned x, const unsigned y, bool is_correct_coord = false);
  Q_INVOKABLE void back_move();
  Q_INVOKABLE QChar letter_return(const unsigned index) const;
  Q_INVOKABLE void start_new_game();
  Q_INVOKABLE void go_to_history_index(const unsigned index);
  Q_INVOKABLE void get_path(const QString& path, bool is_moves_out);

public slots:
  void read_data_from_udp();
  void timer_timeout();

signals:
  void move_turn_color_changed();
  void moves_history_changed();
  void check_mate();

public:
  enum{ZERO_AND_ACTUAL_MOVES = 2, IMG_MID = 40, CELL_SIZE = 560 / 8, a_LETTER = 'a'};
  enum HILIGHT {HILIGHT_CELLS = 2 , FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};
  enum SIMBOLS_IN_STR {FIRST_LETTER = 0, FIRST_NUM = 1, SECOND_LETTER = 5, SECOND_NUM = 6, NEED_SIMBOLS_TO_MOVE = 7};
  enum MESSAGE_TYPE{MOVE = 10, BACK_MOVE, GO_TO_HISTORY, NEW_GAME};
  const QString MOVE_COLOR_W = "img/w_k.png"; const QString MOVE_COLOR_B = "img/b_K.png"; const QString HILIGHT_IM = "hilight";
  const char FREE_SPACE = ' ';

private:
  QTimer *__timer;
  void update_coordinates();
  void switch_move_color();
  void emit_data_changed(const unsigned index);
  void make_move_from_str(const QString& str);
  void send_data_on_server(MESSAGE_TYPE m_type, const int index = 0);
  void correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y, bool is_correct);
  void update_hilight(const Board::Coord& coord, HILIGHT hilight_index);
  void add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to);
  void add_move_to_history_copy(const Board::Coord& coord_from, const Board::Coord& coord_to);
  void moves_from_file(const QString& path);
  void moves_to_file(const QString& path);

private:
  Board* _board;
  UDP_client* _udp_client;
  std::ofstream _in_file;
  QString _path_for_out_moves;
  QString _move_color;
  QStringList _moves_history;
  QList<Figure> _figures_model;
  Board::Coord from;
  Board::Coord to;
  bool _is_message_from_server;

  struct Copy_of_history_moves
  {
    Board::Coord from;
    Board::Coord to;
  };
  std::vector<Copy_of_history_moves> history_copy;
};

class ChessIntegration::Figure
{
public:
  Figure(const QString& name, const int x, const int y, const bool visible);

  inline QString name() const {return _name;}
  inline int x() const {return _x;}
  inline int y() const {return _y;}
  inline bool visible() const {return _visible;}

  inline void set_name(const QString& new_name) {_name = new_name;}
  inline void set_coord(const Board::Coord& new_coord) {_x = new_coord.x; _y = new_coord.y;}
  inline void set_visible(const bool new_visible) {_visible = new_visible;}

private:
  QString _name;
  int _x;
  int _y;
  bool _visible;
};
#endif
