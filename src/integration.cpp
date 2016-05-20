#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <QTimer>
#include "headers/integration.h"
#include "headers/chess.h"
#include "headers/udp_client.h"

ChessIntegration::ChessIntegration(QObject *parent) : QAbstractListModel(parent)
{
  board = new Board();
  udp_client = new UDP_client();
  _move_color = MOVE_COLOR_W;
  _is_message_from_server = false;

  for(int i = 0; i < FIGURES_NUMBER; ++i)
    addFigure(Figure(MOVE_COLOR_W, 0, 0, true));

  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  update_coordinates();

  connect(udp_client, SIGNAL(some_data_came()), this, SLOT(read_data_from_udp()));

  __timer = new QTimer(this);
  connect(__timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  __timer->start(6000);
}

void ChessIntegration::timer_timeout()
{
  __timer->stop();

  static int i = 0;
  ++i;

  if( i == 1)
  {
    move(4,6,true);
    move(4,5,true);
  }
  if( i == 2)
  {
    move(4,1,true);
    move(4,2,true);
  }
  if( i == 3)
  {
    move(1,7,true);
    move(0,5,true);
   //
  }
  __timer->start(6000);
 }

ChessIntegration::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : _name(name), _x(x), _y(y), _visible(visible)
{
}

void ChessIntegration::move(const unsigned x, const unsigned y, bool is_correct_coord)
{
  static bool is_from = true;
  if(!is_check_mate())
  {
    if(is_from)
    {
      correct_figure_coord(from,x,y,is_correct_coord);
      if(board->get_figure(from) != FREE_FIELD)
      {
        update_hilight(from, FIRST_HILIGHT);
        is_from = false;
      }
    }

    else
    {
      is_from = true;
      correct_figure_coord(to, x, y, is_correct_coord);

      if(!(from == to) && board->move(from, to))
      {
        update_hilight(to, SECOND_HILIGHT);
        add_to_history(from, to);

        if(is_check_mate())
          emit check_mate();

        send_data_on_server(MOVE);//udp!!!
      }
      else
      {
        qDebug()<<"====move problem";
        qDebug()<<"from: x = "<<from.x<<" y = "<<from.y;
        qDebug()<<"to: x = "<<to.x<<" y = "<<to.y;
      }
      update_coordinates();
    }
  }
}

void ChessIntegration::back_move()
{
  if(board->back_move())
  {
    qDebug()<<"====back_move";
    update_hilight(board->get_i_from_coord_from_end(1), FIRST_HILIGHT);
    update_hilight(board->get_i_to_coord_from_end(1), SECOND_HILIGHT);
    update_coordinates();
  }
  send_data_on_server(BACK_MOVE);
}

void ChessIntegration::correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y, bool is_correct)
{
  coord.x = is_correct ? x : (x + IMG_MID) / CELL_SIZE;
  coord.y = is_correct ? y : (y + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::update_coordinates()
{
  int index = 0;
  Board::Coord a;
  for(; index < rowCount() - HILIGHT_CELLS; ++index)
  {
    a.x = _figures_model[index].x();
    a.y = _figures_model[index].y();
    if(board->get_figure(a) == FREE_FIELD)
    {
      _figures_model[index].set_visible(false);
      emit_data_changed(index);
    }
  }

  index = 0;
  Board::Coord coord;
  for(coord.y = 0; coord.y < Y_SIZE; ++coord.y)
    for(coord.x = 0; coord.x < X_SIZE; ++coord.x)
      if(board->get_figure(coord) != FREE_FIELD)
      {
        QString fig_name_color;
        if(board->get_color(coord) == W_FIG)
          fig_name_color = "w_";
        else fig_name_color = "b_";
        fig_name_color += board->get_figure(coord);

        _figures_model[index].set_coord(coord);
        _figures_model[index].set_name(fig_name_color);
        _figures_model[index].set_visible(true);

        emit_data_changed(index);
        ++index;
      }
  switch_move_color();
}

void ChessIntegration::update_hilight(const Board::Coord& coord, HILIGHT hilight_index)
{
  _figures_model[hilight_index].set_visible(true);
  _figures_model[hilight_index].set_coord(coord);

  if(hilight_index == FIRST_HILIGHT)
  {
    _figures_model[SECOND_HILIGHT].set_visible(false);
    emit_data_changed(SECOND_HILIGHT);
  }
  emit_data_changed(hilight_index);
}

void ChessIntegration::switch_move_color()
{
  if(board->get_move_color_i_from_end(1) == W_FIG)
    _move_color = MOVE_COLOR_B;
  else  _move_color = MOVE_COLOR_W;

  emit move_turn_color_changed();
}

QString ChessIntegration::move_turn_color() const
{
  return _move_color;
}

void ChessIntegration::emit_data_changed(const unsigned INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

QChar ChessIntegration::letter_return(const unsigned INDEX) const
{
  return QChar(a_LETTER + INDEX);
}

bool ChessIntegration::is_check_mate() const
{
  if(board->get_current_move() <= 2) return false;
  return board->is_mate(board->get_move_color_i_from_end(2));
}

void ChessIntegration::start_new_game()
{
  qDebug()<<"====start_new_game";
  while (board->get_current_move() != 1)
  {
    back_move();
    _moves_history.pop_back();
  }
  _figures_model[FIRST_HILIGHT].set_visible(false);
  emit_data_changed(FIRST_HILIGHT);
  _figures_model[SECOND_HILIGHT].set_visible(false);
  emit_data_changed(SECOND_HILIGHT);
  emit moves_history_changed();
  send_data_on_server(NEW_GAME);
}

void ChessIntegration::go_to_history_index(const unsigned INDEX)
{
  const unsigned CURRENT_MOVE = board->get_current_move() - ZERO_AND_ACTUAL_MOVES;

  if(INDEX == CURRENT_MOVE)
    return;

  if(INDEX < CURRENT_MOVE)
  {
    const unsigned D_INDEX = CURRENT_MOVE - INDEX;

    for(unsigned i = 0; i < D_INDEX; ++i)
      back_move();
  }

  if(INDEX > CURRENT_MOVE)
  {
    for(unsigned i = CURRENT_MOVE; i <= INDEX; ++i)
    {
      board->move(history_copy[i].from, history_copy[i].to);
      update_hilight(history_copy[i].from, FIRST_HILIGHT);
      update_hilight(history_copy[i].to, SECOND_HILIGHT);
      update_coordinates();
    }
  }
  switch_move_color();
  send_data_on_server(GO_TO_HISTORY_INDEX, INDEX);
}

QStringList ChessIntegration::moves_history() const
{
  return _moves_history;
}

void ChessIntegration::add_move_to_history_copy(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  history_copy.shrink_to_fit();
  const int HISTRY_COPY_SIZE = history_copy.size() + ZERO_AND_ACTUAL_MOVES;
  for(int i = board->get_current_move(); i < HISTRY_COPY_SIZE; ++i)
      history_copy.pop_back();

  Copy_of_history_moves copy;

  copy.from = coord_from;
  copy.to = coord_to;

  history_copy.push_back(copy);
}

void ChessIntegration::add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  add_move_to_history_copy(coord_from, coord_to);

  const int LIST_SIZE = _moves_history.size() + ZERO_AND_ACTUAL_MOVES;
  for(int i = board->get_current_move(); i < LIST_SIZE; ++i)
    _moves_history.pop_back();

  QString move;
  QString toY;
  QString fromY;

  move = QChar(a_LETTER + coord_from.x) + fromY.setNum(Y_SIZE - coord_from.y) + " - " + QChar(a_LETTER + coord_to.x) + toY.setNum(Y_SIZE - coord_to.y);

  _moves_history.append(move);
  emit moves_history_changed();
}

//===================================================================================================

void ChessIntegration::read_data_from_udp()
{
  qDebug()<<"=====integrator read_data_from_udp()";
  _is_message_from_server = true;
  QString message;
  udp_client->export_readed_data_to_chess(message);

  switch (message.toInt())
  {
    case BACK_MOVE:
      back_move();
      break;
    case NEW_GAME:
      start_new_game();
      break;
    default:
      message[0].isNumber() ? go_to_history_index_from_server(message) : make_move_from_str(message);
  }
}

void ChessIntegration::make_move_from_str(const QString& str)
{
  move((str[FIRST_LETTER].unicode() - a_LETTER), (Y_SIZE - str[FIRST_NUM].digitValue()), true);
  move((str[SECOND_LETTER].unicode() - a_LETTER), (Y_SIZE - str[SECOND_NUM].digitValue()), true);
}

void ChessIntegration::go_to_history_index_from_server(QString& str)
{
  for(int i = 0; i < str.size(); ++i)
  {
    if(str[0] == FREE_SPACE)
    {
      str.remove(0,1);
      break;
    }
    else str.remove(0,1);
  }

  qDebug()<<"go to history index from server: "<<str.toInt();
  if(str.size() > 0)
    go_to_history_index(str.toInt());
}

void ChessIntegration::send_data_on_server(MESSAGE_TYPE m_type, const unsigned INDEX)
{
  qDebug()<<"send_data_on_server";
  if(_is_message_from_server)
  {
    _is_message_from_server = false;
    qDebug()<<"message from server";
    return;
  }

  QByteArray message;
  if(m_type == MOVE)
    message.append(_moves_history[_moves_history.size() -1]);
  else if(m_type == GO_TO_HISTORY_INDEX)
      message.append(INDEX);
  else message.setNum(m_type);

  udp_client->send_data(message);
}

//===========================================================================================================

void ChessIntegration::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _figures_model << figure;
  endInsertRows();
}

int ChessIntegration::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return _figures_model.count();
}

QVariant ChessIntegration::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= _figures_model.count())
    return QVariant();

  const Figure &figure = _figures_model[index.row()];
  if (role == NameRole)
    return figure.name();
  else if (role == XRole)
    return figure.x();
  else if (role == YRole)
    return figure.y();
  else if (role == VisibleRole)
    return figure.visible();
  return QVariant();
}

QHash<int, QByteArray> ChessIntegration::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}
