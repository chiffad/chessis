#ifndef PIECHART_H
#define PIECHART_H

#include <QtQuick/QQuickPaintedItem>
#include <QString>
#include <QChar>
#include <QAbstractListModel>
#include <QStringList>
#include <vector>
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
  ~ChessIntegration(){delete board; delete udp_client;}

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
  Q_INVOKABLE QChar letter_return(const int index) const;
  Q_INVOKABLE void start_new_game();
  Q_INVOKABLE void go_to_history_index(unsigned index);

public slots:
  void read_data_from_udp();//udp!!!!

signals:
  void move_turn_color_changed();
  void moves_history_changed();
  void check_mate();

private:
  enum{ZERO_AND_ACTUAL_MOVES = 2, IMG_MID = 40, CELL_SIZE = 560 / 8, a_LETTER = 'a'};
  enum HILIGHT {HILIGHT_CELLS = 2 , FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};
  enum SIMBOLS_IN_STR {FIRST_LETTER = 0, FIRST_NUM = 1, SECOND_LETTER = 5, SECOND_NUM = 6, NEED_SIMBOLS_TO_MOVE = 7};
  enum MESSAGE_TYPE{MOVE, BACK_MOVE, NEW_GAME, REPEAT_MESSAGE, MESSAGE_RECEIVED};
  const QString MOVE_COLOR_W = "img/w_k.png"; const QString MOVE_COLOR_B = "img/b_K.png"; const QString HILIGHT_IM = "hilight";

private:
  void update_coordinates();
  void switch_move_color();
  void emit_data_changed(const int INDEX);
  void make_move_from_str(const QString& str);//udp!!!!
  void send_data_on_server(MESSAGE_TYPE m_type);//udp!!!!
  void correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y, bool is_correct);
  void update_hilight(const Board::Coord& coord, HILIGHT hilight_index);
  void add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to);
  void add_move_to_history_copy(const Board::Coord& coord_from, const Board::Coord& coord_to);

private:
  Board* board;
  UDP_client* udp_client;//udp!!!!
  QString m_move_color;
  QStringList m_moves_history;
  QList<Figure> m_figures_model;
  Board::Coord from;
  Board::Coord to;
  QString m_udp_data;//udp!!!!
  bool m_is_message_from_server;
  bool m_is_opponent_received_message;
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

  inline QString name() const {return m_name;}
  inline int x() const {return m_x;}
  inline int y() const {return m_y;}
  inline bool visible() const {return m_visible;}

  inline void set_name(const QString& new_name) {m_name = new_name;}
  inline void set_coord(const Board::Coord& new_coord) {m_x = new_coord.x; m_y = new_coord.y;}
  inline void set_visible(const bool new_visible) {m_visible = new_visible;}

private:
  QString m_name;
  int m_x;
  int m_y;
  bool m_visible;
};
#endif
