#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <QTime>
#include "headers/integration.h"
#include "headers/chess.h"
#include "headers/udp_client.h"

ChessIntegration::ChessIntegration(QObject *parent) : QAbstractListModel(parent)
{
  board = new Board();
  udp_client = new UDP_client(); //udp!!!!
  m_is_message_from_server = false;
  m_is_opponent_received_message = false;
  m_last_received_message_num = 0;
  m_send_message_num = 0;
  m_move_color = MOVE_COLOR_W;

  for(int i = 0; i < FIGURES_NUMBER; ++i)
    addFigure(Figure(MOVE_COLOR_W, 0, 0, true));

  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  update_coordinates();

  connect(udp_client, SIGNAL(some_data_came()), this, SLOT(read_data_from_udp()));//udp!!!!
}

ChessIntegration::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : m_name(name), m_x(x), m_y(y), m_visible(visible)
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
      correct_figure_coord(to, x, y,is_correct_coord);

      if(!(from == to) && board->move(from, to))
      {
        update_hilight(to, SECOND_HILIGHT);
        add_to_history(from, to);

        if(is_check_mate())
          emit check_mate();

        m_is_moved = true;
        send_data_on_server(MOVE);//udp!!!
      }
      else m_is_moved = false;
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
  send_data_on_server(BACK_MOVE);//udp!!!!
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
    a.x = m_figures_model[index].x();
    a.y = m_figures_model[index].y();
    if(board->get_figure(a) == FREE_FIELD)
    {
      m_figures_model[index].set_visible(false);
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

        m_figures_model[index].set_coord(coord);
        m_figures_model[index].set_name(fig_name_color);
        m_figures_model[index].set_visible(true);

        emit_data_changed(index);
        ++index;
      }
  switch_move_color();
}

void ChessIntegration::update_hilight(const Board::Coord& coord, HILIGHT hilight_index)
{
  m_figures_model[hilight_index].set_visible(true);
  m_figures_model[hilight_index].set_coord(coord);

  if(hilight_index == FIRST_HILIGHT)
  {
    m_figures_model[SECOND_HILIGHT].set_visible(false);
    emit_data_changed(SECOND_HILIGHT);
  }
  emit_data_changed(hilight_index);
}

void ChessIntegration::switch_move_color()
{
  if(board->get_move_color_i_from_end(1) == W_FIG)
    m_move_color = MOVE_COLOR_B;
  else  m_move_color = MOVE_COLOR_W;

  emit move_turn_color_changed();
}

QString ChessIntegration::move_turn_color() const
{
  return m_move_color;
}

void ChessIntegration::emit_data_changed(const int INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

QChar ChessIntegration::letter_return(const int index) const
{
  return QChar(a_LETTER + index);
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
    m_moves_history.pop_back();
  }
  m_figures_model[FIRST_HILIGHT].set_visible(false);
  emit_data_changed(FIRST_HILIGHT);
  m_figures_model[SECOND_HILIGHT].set_visible(false);
  emit_data_changed(SECOND_HILIGHT);
  emit moves_history_changed();
  send_data_on_server(NEW_GAME);//udp!!!!
}

void ChessIntegration::go_to_history_index(unsigned index)
{
  const unsigned CURRENT_MOVE = board->get_current_move();
  if(index == CURRENT_MOVE - ZERO_AND_ACTUAL_MOVES) return;

  if(index < CURRENT_MOVE - ZERO_AND_ACTUAL_MOVES)
  {
    const unsigned D_INDEX = CURRENT_MOVE - index - ZERO_AND_ACTUAL_MOVES;

    for(unsigned i = 0; i < D_INDEX; ++i)
      back_move();
  }

  if(index > CURRENT_MOVE - ZERO_AND_ACTUAL_MOVES)
  {
    for(unsigned i = CURRENT_MOVE - ZERO_AND_ACTUAL_MOVES ; i <= index; ++i)
    {
      board->move(history_copy[i].from, history_copy[i].to);
      update_hilight(history_copy[i].from, FIRST_HILIGHT);
      update_hilight(history_copy[i].to, SECOND_HILIGHT);
      update_coordinates();
    }
  }
  switch_move_color();
}

QStringList ChessIntegration::moves_history() const
{
  return m_moves_history;
}

void ChessIntegration::add_move_to_history_copy(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  history_copy.shrink_to_fit();
  const unsigned HISTRY_COPY_SIZE = history_copy.size() + ZERO_AND_ACTUAL_MOVES;
  for(unsigned i = board->get_current_move(); i < HISTRY_COPY_SIZE; ++i)
      history_copy.pop_back();

  Copy_of_history_moves copy;

  copy.from = coord_from;
  copy.to = coord_to;

  history_copy.push_back(copy);
}

void ChessIntegration::add_to_history(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  add_move_to_history_copy(coord_from, coord_to);

  const int LIST_SIZE = m_moves_history.size() + ZERO_AND_ACTUAL_MOVES;
  for(int i = board->get_current_move(); i < LIST_SIZE; ++i)
    m_moves_history.pop_back();

  unsigned correct_move = board->get_current_move() / 2;
  QString move;
  QString toY;
  QString fromY;

  if(board->get_current_move() % 2 == 0)
  {
    move.setNum(correct_move);
    move += " ";
  }
  move += QChar(a_LETTER + coord_from.x) + fromY.setNum(Y_SIZE - coord_from.y) + " - " + QChar(a_LETTER + coord_to.x) + toY.setNum(Y_SIZE - coord_to.y);

  m_moves_history.append(move);
  emit moves_history_changed();
}

//===================================================================================================

void ChessIntegration::read_data_from_udp()//udp!!!!
{
  QString message;
  udp_client->export_readed_data_to_chess(message);
  m_is_message_from_server = true;

  if(message.toInt() != MESSAGE_RECEIVED)
    send_data_on_server(MESSAGE_RECEIVED);

  qDebug()<<"====read_data_from_udp: "<<message;
  QString serial_num;
  while(message.size() > 0 && QString(message[0]) != FREE_SPASE)
  {
    serial_num.append(message[0]);
    message.remove(0,1);
  }

  while(message.size() > NEED_SIMBOLS_TO_MOVE)
    message.remove(0,1);

  qDebug()<<"serial_num.toInt()"<<serial_num.toInt()<<" ;m_last_received_messag: "<<m_last_received_message_num;
  if(message.size() <= 0 || serial_num.toInt() != m_last_received_message_num + 1)
  {
    qDebug()<<"wrong message";
    //send_data_on_server(REPEAT_MESSAGE);
    return;
  }

  ++m_last_received_message_num;
  switch (message.toInt())
  {
    case BACK_MOVE:
      back_move();
      break;
    case NEW_GAME:
      start_new_game();
      break;
    case REPEAT_MESSAGE:
      send_data_on_server(REPEAT_MESSAGE);
      break;
    case MESSAGE_RECEIVED:
      --m_last_received_message_num;
      m_is_opponent_received_message = true;
      break;
    default:
      make_move_from_str(message);
  }
}

void ChessIntegration::make_move_from_str(const QString& str)//udp!!!!
{
  qDebug()<<"==== make_move_from_str: "<<str;
  if(str.size() != NEED_SIMBOLS_TO_MOVE)
  {
    send_data_on_server(REPEAT_MESSAGE);
    return;
  }
  move((str[FIRST_LETTER].unicode() - a_LETTER), (Y_SIZE - str[FIRST_NUM].digitValue()), true);
  move((str[SECOND_LETTER].unicode() - a_LETTER), (Y_SIZE - str[SECOND_NUM].digitValue()), true);

  if(!m_is_moved)
    send_data_on_server(REPEAT_MESSAGE);
}

void ChessIntegration::send_data_on_server(MESSAGE_TYPE m_type)//udp!!!!
{
    qDebug()<<"here";
  if(!m_is_message_from_server || m_type == REPEAT_MESSAGE || m_type == MESSAGE_RECEIVED)
  {
    QByteArray message;
    message.setNum(++m_send_message_num);
    message.append(FREE_SPASE);

    if(m_type == MOVE || m_type == REPEAT_MESSAGE)
      message.append(m_moves_history[m_moves_history.size() -1]);
    else message.setNum(m_type);

    udp_client->send_data(message);

    /*if(m_type != MESSAGE_RECEIVED)
    {
      QTime timer;
      timer.start();
      const int SECOND = 1000;
      while(udp_client->is_server_connected() && udp_client->is_have_opponent() && !m_is_opponent_received_message)
        if(timer.elapsed() > SECOND)
        {
          timer.restart();
          udp_client->send_data(message);
        }
    }*/
    //m_is_opponent_received_message = false;
  }
  else m_is_message_from_server = false;
}

//===========================================================================================================

void ChessIntegration::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_figures_model << figure;
  endInsertRows();
}

int ChessIntegration::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return m_figures_model.count();
}

QVariant ChessIntegration::data(const QModelIndex & index, int role) const
{
  if (index.row() < 0 || index.row() >= m_figures_model.count())
    return QVariant();

  const Figure &figure = m_figures_model[index.row()];
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
