#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <QByteArray>
#include <QTime>
#include "headers/integration.h"
#include "headers/chess.h"
#include "headers/udp_client.h"

ChessIntegration::ChessIntegration(QObject *parent)
    : QAbstractListModel(parent)
{
  board = new Board();
  udp_client = new UDP_client(); //udp!!!!
  m_is_message_from_server = false;
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

        create_and_send_data_on_server(MOVE);//udp!!!

        m_is_move_was_made = true;
      }
      else m_is_move_was_made = false;
      update_coordinates();
    }
  }
}

void ChessIntegration::back_move()
{
  if(board->back_move())
  {
    update_hilight(board->get_i_from_coord_from_end(1), FIRST_HILIGHT);
    update_hilight(board->get_i_to_coord_from_end(1), SECOND_HILIGHT);
    update_coordinates();
  }
  create_and_send_data_on_server(BACK_MOVE);//udp!!!!
}

void ChessIntegration::correct_figure_coord(Board::Coord& coord, const unsigned x, const unsigned y, bool is_correct)
{
  coord.x = is_correct ? x : (x + IMG_MID) / CELL_SIZE;
  coord.y = is_correct ? y : (y + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::update_coordinates()
{
  int index = 0;
  Board::Coord coord;
  for(; index < rowCount() - HILIGHT_CELLS; ++index)
  {
    coord.x = m_figures_model[index].x();
    coord.y = m_figures_model[index].y();
    if(board->get_figure(coord) == FREE_FIELD)
    {
      m_figures_model[index].set_visible(false);
      emit_data_changed(index);
    }
  }

  index = 0;
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
  create_and_send_data_on_server(NEW_GAME);//udp!!!!
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

//=================================================================================================

void ChessIntegration::read_data_from_udp()//udp!!!!
{
  qDebug()<<"read_data_from_udp";
  QString udp_data;
  udp_client->export_readed_data_to_chess(udp_data);

  if(NEED_SIMBOLS_TO_MOVE != udp_data.size() - 1)
    create_and_send_data_on_server(REPEAT_MESSAGE);

  else
  {
    m_is_oponent_recived_move = false;

    m_is_message_from_server = true;

    qDebug()<<"message.toInt(): "<<udp_data.toInt()<<" ;message: "<<udp_data<<" ;BACK_MOVE: "<<BACK_MOVE;
    if(udp_data.toInt() == BACK_MOVE)
      back_move();
    else if(udp_data.toInt() == NEW_GAME)
      start_new_game();
    else if(udp_data.toInt() == MOVE_RECIVED)
      m_is_oponent_recived_move = true;
    else if(udp_data.toInt() == REPEAT_MESSAGE)
      create_and_send_data_on_server(MOVE_AGAIN);
    else make_move_from_str(udp_data);
  }
}

void ChessIntegration::make_move_from_str(const QString& str)//udp!!!!
{
  qDebug()<<"make_move_from_str";
  move((str[FIRST_LETTER].unicode() - a_LETTER), (Y_SIZE - str[FIRST_NUM].digitValue()), true);
  move((str[SECOND_LETTER].unicode() - a_LETTER), (Y_SIZE - str[SECOND_NUM].digitValue()), true);

  if(!m_is_move_was_made)
    create_and_send_data_on_server(REPEAT_MESSAGE);
  else create_and_send_data_on_server(MOVE_RECIVED);
}

void ChessIntegration::create_and_send_data_on_server(MESSAGE_TYPE m_type)//udp!!!!
{  
  if(!m_is_message_from_server || m_type == REPEAT_MESSAGE || m_type == MOVE_AGAIN || m_type == MOVE_RECIVED)
  {
    QString message;
    if(m_type == MOVE || m_type == MOVE_AGAIN)
      message = m_moves_history[m_moves_history.size() -1];
    else message = m_type;

    QByteArray m;
    m.append(message);
    qDebug()<<"sended message: "<<message;

    udp_client->send_data(m);

    QTime timer;
    timer.start();
    const int ONE_SECOND = 1000;
    while(timer.elapsed() < ONE_SECOND);
    if(!m_is_oponent_recived_move)
    {
      qDebug()<<"oponent didn't recive move";
      create_and_send_data_on_server(REPEAT_MESSAGE);
    }
  }
  m_is_message_from_server = false;
}

//==================================================================================================

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
