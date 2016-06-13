#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <fstream>
#include <sstream>
#include "headers/integration.h"
#include "headers/udp_client.h"


ChessIntegration::ChessIntegration(QObject *parent) : QAbstractListModel(parent), _move_color(MOVE_COLOR_W),
                                                     _udp_connection_status(DISCONNECT), _is_message_from_server(false)
{
  _udp_client = new UDP_client();

  for(int i = 0; i < Board::FIGURES_NUMBER; ++i)
    addFigure(Figure(MOVE_COLOR_W, 0, 0, true));

  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  addFigure(ChessIntegration::Figure(HILIGHT_IM, 0, 0, false));
  update_coordinates();

  connect(_udp_client, SIGNAL(some_data_came()), this, SLOT(read_data_from_udp()));

  timer_kill = new QTimer(this);
  connect(timer_kill, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  timer_kill->start(1000);
}

void ChessIntegration::timer_timeout()
{
  timer_kill->stop();

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
  }

  if( i == 4)
  {
     go_to_history_index(0);
  }

  if( i == 5)
  {
     go_to_history_index(2);
  }

  timer_kill->start(1000);
}


ChessIntegration::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : _name(name), _x(x), _y(y), _visible(visible)
{
}

void ChessIntegration::move(const unsigned x, const unsigned y, bool is_correct_coord)
{
  static bool is_from = true;

  if(is_from)
  {
    correct_figure_coord(from,x,y,is_correct_coord);
    if(_board->get_figure(from) != Board::FREE_FIELD)
    {
      update_hilight(from, FIRST_HILIGHT);
      is_from = false;
    }
  }

  else
  {
    is_from = true;
    correct_figure_coord(to, x, y, is_correct_coord);

    if(_board->move(from, to))
    {
      update_hilight(to, SECOND_HILIGHT);
      add_move_to_str_history(from, to);

      is_check_mate();

      send_data_on_server(MOVE);
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

void ChessIntegration::back_move()
{
  if(_board->back_move())
  {
    qDebug()<<"====back_move";
    update_hilight(_board->get_prev_from_coord(), FIRST_HILIGHT);
    update_hilight(_board->get_prev_to_coord(), SECOND_HILIGHT);
    update_coordinates();
    send_data_on_server(BACK_MOVE);
  }
}

void ChessIntegration::correct_figure_coord(Coord& coord, const unsigned x, const unsigned y, bool is_correct)
{
  coord.x = is_correct ? x : (x + IMG_MID) / CELL_SIZE;
  coord.y = is_correct ? y : (y + IMG_MID) / CELL_SIZE;
}

void ChessIntegration::update_coordinates()
{
  int index = 0;
  Coord a;
  for(; index < rowCount() - HILIGHT_CELLS; ++index)
  {
    a.x = _figures_model[index].x();
    a.y = _figures_model[index].y();
    if(_board->get_figure(a) == Board::FREE_FIELD)
    {
      _figures_model[index].set_visible(false);
      emit_data_changed(index);
    }
  }

  index = 0;
  Coord coord;
  for(coord.y = 0; coord.y < Board::Y_SIZE; ++coord.y)
    for(coord.x = 0; coord.x < Board::X_SIZE; ++coord.x)
      if(_board->get_figure(coord) != Board::FREE_FIELD)
      {
        QString fig_name_color = _board->get_color(coord) == Board::W_FIG ? "w_" : "b_";
        fig_name_color += _board->get_figure(coord);

        _figures_model[index].set_coord(coord);
        _figures_model[index].set_name(fig_name_color);
        _figures_model[index].set_visible(true);

        emit_data_changed(index);
        ++index;
      }
  switch_move_color();
}

void ChessIntegration::update_hilight(const Coord& coord, HILIGHT hilight_index)
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
  if(_board->get_move_color_i_from_end(1) == Board::W_FIG)
    _move_color = MOVE_COLOR_B;
  else  _move_color = MOVE_COLOR_W;

  emit move_turn_color_changed();
}
void ChessIntegration::emit_data_changed(const unsigned INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool ChessIntegration::is_check_mate() const
{
  if(!_board->is_mate(_board->get_move_color_i_from_end(2)))
    return false;

  emit check_mate();
  return true;
}

void ChessIntegration::start_new_game()
{
  qDebug()<<"====start_new_game";
  while(_board->get_current_move() != 1)
  {
    back_move();
    _str_moves_history.pop_back();
  }
  _figures_model[FIRST_HILIGHT].set_visible(false);
  emit_data_changed(FIRST_HILIGHT);
  _figures_model[SECOND_HILIGHT].set_visible(false);
  emit_data_changed(SECOND_HILIGHT);
  emit moves_history_changed();
  send_data_on_server(NEW_GAME);
}

void ChessIntegration::go_to_history_index(const unsigned index)
{
  qDebug()<<"====go to history index: " <<index;

  if(_board->go_to_history_index(index))
  {
    //update_hilight(history_copy[i].from, FIRST_HILIGHT);
    //update_hilight(history_copy[i].to, SECOND_HILIGHT);
    update_coordinates();
    send_data_on_server(GO_TO_HISTORY, index);
    switch_move_color();
  }
}

void ChessIntegration::add_move_to_str_history(const Board::Coord& coord_from, const Board::Coord& coord_to)
{
  const int LIST_SIZE = _str_moves_history.size() + ZERO_AND_ACTUAL_MOVES;
  for(int i = _board->get_current_move(); i < LIST_SIZE; ++i)
    _str_moves_history.pop_back();

  QString move = QChar(a_LETTER + coord_from.x) + QString::number(Board::Y_SIZE - coord_from.y) + " - "
                 + QChar(a_LETTER + coord_to.x) + QString::number(Board::Y_SIZE - coord_to.y);

  _str_moves_history.append(move);
  emit moves_history_changed();
}

void ChessIntegration::run_command(const QString& command)
{
  qDebug()<<"===run_command: "<<command;
  _commands_history.append(command);

  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString SHOW_OPPONENT = "show opponent";
  const QString SHOW_ME = "show me";

  if(command == HELP_WORD)
  {
    qDebug()<<"help_word";
    add_to_comman_history("For move type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + "d2-d4).");
    add_to_comman_history("To see opponent information, print '" + SHOW_OPPONENT + " .");
    add_to_comman_history("To view your information, print '" + SHOW_ME + " .");
  }
  else if(command == SHOW_OPPONENT)
    send_data_on_server(OPPONENT_INF_REQUEST);
  else if(command == SHOW_ME)
    send_data_on_server(MY_INF_REQUEST);
  else
  {
    QString command_copy = command;
    QString first_word;
    while(command_copy.size())
    {
      if(command_copy[0].isLetter())
        first_word.append(command_copy[0]);

      command_copy.remove(0,1);
      if(first_word.size() == 4)
        break;
    }
    if(first_word == MOVE_WORD)
    {
      make_move_from_str(command_copy);
      qDebug()<<"move word";
    }
    else add_to_comman_history("Unknown command ('" + HELP_WORD + "' for help).");
  }
}

void ChessIntegration::add_to_comman_history(const QString& str)
{
  _commands_history.append(str);
  emit commands_list_changed();
}

void ChessIntegration::path_to_file(QString& path, bool is_moves_from_file)
{
  path.remove(0,7);
  qDebug()<<"===path_to_file: "<<path;
  is_moves_from_file ? read_moves_from_file(path) : write_moves_to_file(path);
}

void ChessIntegration::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toUtf8().constData());
  for(int i = 0; i < _str_moves_history.size(); ++i)
  {
    in_file<<_str_moves_history[i].toUtf8().constData();
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void ChessIntegration::read_moves_from_file(const QString& path)
{
  qDebug()<<"===read_move_from_file: "<<path;
  std::ifstream from_file(path.toUtf8().constData());
  std::ostringstream out;
  out<<from_file.rdbuf();

  std::string data_from_file;
  out.str().swap(data_from_file);

  from_file.close();

  start_new_game();
  std::cout<<"data_from_file std: "<<data_from_file<<std::endl;
  qDebug()<<QString::fromStdString(data_from_file);
  make_move_from_str(QString::fromStdString(data_from_file));
}

void ChessIntegration::make_move_from_str(const QString& str)
{
  qDebug()<<"==make_move_from_str CHESS: "<<str;
  QVector<int> coord_str;
  enum{FROM_X = 0, FROM_Y = 1, TO_X = 2, TO_Y = 3, COORD_NEED_TO_MOVE = 4};

  for(int i = 0; i < str.size(); ++i)
  {
    if(!str[i].isLetterOrNumber())
      continue;

    int coord = str[i].isNumber() ? Board::Y_SIZE - str[i].digitValue() : str[i].unicode() - a_LETTER;

    if(coord < Board::X_SIZE)
      coord_str.push_back(coord);

    if(coord_str.size() == COORD_NEED_TO_MOVE)
    {
      move(coord_str[FROM_X], coord_str[FROM_Y], true);
      move(coord_str[TO_X], coord_str[TO_Y], true);
      coord_str.clear();
    }
  }
}

void ChessIntegration::set_connect_status(const QString& status)
{
  if(_udp_connection_status == status)
    return;
  _udp_connection_status = status;
  emit udp_connection_status_changed();
}

//===================================================================================================

void ChessIntegration::read_data_from_udp()
{
  _is_message_from_server = true;
  QString message;
  _udp_client->export_readed_data_to_chess(message);

  qDebug()<<"=====integrator read_data_from_udp(): "<<message;

  QString message_type;
  while(message.size())
  {
    while(message[0].isLetterOrNumber())
    {
      message_type.append(message[0]);
      message.remove(0,1);
    }
    message.remove(0,1);
    if(message_type.size())
      break;
  }

  switch(message_type.toInt())
  {
    case SERVER_LOST:
      set_connect_status(DISCONNECT);
      break;
    case OPPONENT_LOST:
      set_connect_status(OPPONENT_DISCONNECT);
      break;
    case SERVER_HERE:
      set_connect_status(CONNECT);
      break;
    case BACK_MOVE:
      back_move();
      break;
    case NEW_GAME:
      start_new_game();
      break;
    case OPPONENT_INF_REQUEST:
      add_to_comman_history(message);
      if(_udp_connection_status == DISCONNECT)
        set_connect_status(CONNECT);
      break;
    case GO_TO_HISTORY:
      go_to_history_index(message.toInt());
      break;
    default:
      make_move_from_str(message);
  }
  if(message_type.toInt() >= MOVE && message_type.toInt() <= NEW_GAME)
    set_connect_status(CONNECT);
}

void ChessIntegration::send_data_on_server(MESSAGE_TYPE type, const int index)
{
  qDebug()<<"====send_data_on_server_chess";
  if(_is_message_from_server)
  {
    _is_message_from_server = false;
    qDebug()<<"message from server";
    return;
  }

  QByteArray message;
  message.setNum(type);

  if((type == MOVE && !_str_moves_history.isEmpty()) || type == GO_TO_HISTORY)
  {
    message += FREE_SPACE;
    message += (type == MOVE) ? _str_moves_history.last() : QByteArray::number(index);
  }
  _udp_client->send_data(message);
}

//===========================================================================================================

QStringList ChessIntegration::moves_history() const
{
  return _str_moves_history;
}

QStringList ChessIntegration::commands_list() const
{
  return _commands_history;
}

QString ChessIntegration::move_turn_color() const
{
  return _move_color;
}

QString ChessIntegration::udp_connection_status() const
{
  return _udp_connection_status;
}

QChar ChessIntegration::letter_return(const unsigned index) const
{
  return QChar(a_LETTER + index);
}

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
