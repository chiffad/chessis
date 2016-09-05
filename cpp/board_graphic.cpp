#include <QPainter>
#include <QString>
#include <QChar>
#include <ctype.h>
#include <QModelIndex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "board_graphic.h"
#include "enums.h"

Board_graphic::Board_graphic(QObject *parent) : QAbstractListModel(parent), _move_color(MOVE_COLOR_W),
                                                _udp_connection_status("Disconnected"), _is_check_mate(false)
{
  for(int i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS; ++i)
    addFigure(Figure("b_P", 0, 0, false));
 //   addFigure(Figure(HILIGHT_IM, 0, 0, false));

  timer_kill = new QTimer(this);
  connect(timer_kill, SIGNAL(timeout()), this, SLOT(timer_timeout()));
  //timer_kill->start(10000);
}

Board_graphic::~Board_graphic()
{
  delete timer_kill;
}

Board_graphic::Figure::Figure(const QString& name, const int x, const int y, const bool visible)
    : _name(name), _x(x), _y(y), _visible(visible)
{
}

Board_graphic::Figure::~Figure()
{
}

void Board_graphic::timer_timeout()
{
  timer_kill->stop();

  static int i = 0;
  ++i;

  if( i == 1)
  {
    run_command(MOVE_WORD, 4 * CELL_SIZE_X,6*CELL_SIZE_Y, 4 * CELL_SIZE_X,4 * CELL_SIZE_Y);
  }
  if( i == 2)
  {
    run_command(MOVE_WORD,4*CELL_SIZE_X,1*CELL_SIZE_Y ,4*CELL_SIZE_X,2*CELL_SIZE_Y);
  }

  if( i == 3)
  {
    run_command(MOVE_WORD,1*CELL_SIZE_X,7*CELL_SIZE_Y, 0*CELL_SIZE_X,5*CELL_SIZE_Y);
  }

 /* if( i == 1)
  {
    QString s("/home/dprokofiev/prj/!chess/chess_hist.txt");
    path_to_file(s, true);
     //run_command("new game");
  }*/
/*  if( i == 4)
  {
     run_command("to history",2);
  }

  if( i == 5)
  {
     run_command("to history",1);
  }

  if( i == 6)
  {
     run_command("to history",0);
  }

  if( i == 7)
  {
     run_command("to history",1);
  }

  if( i == 8)
  {
     run_command("to history",2);
  }*/

  timer_kill->start(5000);
}

void Board_graphic::run_command(const QString& message, const int x1, const int y1, const int x2, const int y2)
{
  qDebug()<<"Board_graphic::run_command: "<<message<<" x1: "<<x1<< "y1"<< y1<<" x2: "<<x2<< "y2"<< y2;
  add_to_command_history("command: " + message);

  if(message == HELP_WORD)
  {
    qDebug()<<"help_word";
    add_to_command_history("1.For move, type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + " d2-d4)" + "\n"+
    "2.For back move, type '" + BACK_MOVE + "'" + "\n"+
    "3.For start new game, type '" + NEW_GAME + "'" + "\n"+
    "4.For go to history index, type '" + HISTORY + "' and index" + "\n"+
    "5.To view opponent information, print '" + SHOW_OPPONENT + "'" + "\n"+
    "6.To view your information, print '" + SHOW_ME + "'");
  }
  else if(message == SHOW_OPPONENT)
  {
    qDebug()<<"show opponent";
    add_to_messages_for_server_stack(Messages::OPPONENT_INF_REQUEST);
  }
  else if(message == SHOW_ME)
  {
    add_to_messages_for_server_stack(Messages::MY_INF_REQUEST);
    qDebug()<<"show me";
  }
  else if(message == NEW_GAME)
  {
    qDebug()<<"new game";
    add_to_messages_for_server_stack(Messages::NEW_GAME);
  }
  else if(message == BACK_MOVE)
  {
    qDebug()<<"back_move";
    add_to_messages_for_server_stack(Messages::BACK_MOVE);
  }
  else if(message == HISTORY)
  {
    qDebug()<<"history";
    add_to_messages_for_server_stack(Messages::GO_TO_HISTORY, QString::number(x1));
  }
  else
  {
    const QString command(message.mid(0,message.indexOf(FREE_SPACE)));
    const QString command_content(message.mid(command.size()));

    qDebug()<<"command:"<<command;
    qDebug()<<"content:"<<command_content;

    if(command == MOVE_WORD)
    {
      qDebug()<<"move word";
      if(!command_content.isEmpty())
        add_to_messages_for_server_stack(Messages::MOVE, command_content);
      else
      {
        Coord from,to;
        if(set_correct_coord(from, x1, y1) && set_correct_coord(to, x2, y2))
          add_to_messages_for_server_stack(Messages::MOVE, coord_to_str(from, to));
      }
    }
    else add_to_command_history("Unknown command (type '" + HELP_WORD + "' for help).");
  }
}

bool Board_graphic::set_correct_coord(Coord& c, const int x, const int y)
{
  if(x < 0 || x > (CELL_SIZE_X * CELL_NUM) || y < 0 || y > (CELL_SIZE_Y * CELL_NUM))
    return false;

  qDebug()<<"Board_graphic::set_correct_coord true";
  c.x = (x + (CELL_SIZE_X / 2)) / CELL_SIZE_X;
  c.y = (y + (CELL_SIZE_Y / 2)) / CELL_SIZE_Y;

  return true;
}

void Board_graphic::update_coordinates()
{
  qDebug()<<"Board_graphic::update_coord";
  auto f_it = _field.begin();
  for(auto &fig_mod : _figures_model)
  {
    f_it = std::find_if(f_it, _field.end(), [](auto const &i) {return i != FREE_FIELD;});
    if(f_it != _field.end())
    {
      fig_mod.set_coord(get_field_coord(_field.indexOf(*f_it, (f_it - _field.begin()))));
      fig_mod.set_name(QString(f_it->isLower() ? "w_" : "b_") + *f_it);
      fig_mod.set_visible(true);
      ++f_it;
    }
    else fig_mod.set_visible(false);

    emit_figure_changed(_figures_model.indexOf(fig_mod));
  }
}

Board_graphic::Coord Board_graphic::get_field_coord(const int i) const
{
  Coord c;
  c.x = i % CELL_NUM;
  c.y = i / CELL_NUM;
  return c;
}

void Board_graphic::set_board_mask(const QString& mask)
{
  qDebug()<<"Board_graphic::set_board_mask";
  if(mask.size() != CELL_NUM * CELL_NUM)
  {
    qDebug()<<"mask is wrong!"<<mask;
    return;
  }
  _field.clear();
  _field = mask;

  update_coordinates();
}

void Board_graphic::set_moves_history(const QString& history)
{
  qDebug()<<"Board_graphic::set_moves_history"<<history;
  _str_moves_history.clear();

  QString move;
  for(const auto &simb : history)
  {
    move.append(simb);

    if(move.size() == NEED_SIMB_TO_MOVE)
    {
      move.insert(2, " - ");
      _str_moves_history.append(move);
      move.clear();
    }
  }

  if(!history.isEmpty())
  {
    auto r_simb = history.rbegin();
    Coord coord;
    for(int i = 0; i < 2; ++i)
    {
      coord.y = CELL_NUM - (*(r_simb++)).digitValue();
      coord.x = (*(r_simb++)).unicode() - a_LETTER;
      update_hilight(coord, ((i == 0) ? SECOND_HILIGHT : FIRST_HILIGHT));
    }
  }

  emit moves_history_changed();
}

void Board_graphic::set_move_color(const int move_num)
{
  qDebug()<<"Board_graphic::set_move_color";
  _move_color = (move_num % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

const QString Board_graphic::coord_to_str(const Coord &from, const Coord &to) const
{
  qDebug()<<"Board_graphic::coord_to_str:"<<  (QChar(a_LETTER + from.x) + QString::number(CELL_NUM - from.y)
                              + " - " + QChar(a_LETTER + to.x) + QString::number(CELL_NUM - to.y));
  return (QChar(a_LETTER + from.x) + QString::number(CELL_NUM - from.y)
          + " - " + QChar(a_LETTER + to.x) + QString::number(CELL_NUM - to.y));
}

void Board_graphic::add_to_messages_for_server_stack(const Messages::MESSAGE mes_type, const QString& content)
{
  qDebug()<<"Board_graphic::add_to_messages_for_server_stack";
  _messages_for_server_stack.append(QString::number(mes_type) + FREE_SPACE + content);
  qDebug()<<"!!!!!_messages_for_server_stack.last"<<_messages_for_server_stack.last();
}

void Board_graphic::update_hilight(const Coord &coord, const enum HILIGHT hilight_index)
{
  qDebug()<<"Board_graphic::update_hilight; "<<coord.x<<" "<<coord.y;

  _figures_model[hilight_index].set_visible(true);
  _figures_model[hilight_index].set_coord(coord);
  emit_figure_changed(hilight_index);
}

void Board_graphic::emit_figure_changed(const unsigned INDEX)
{
  QModelIndex topLeft = index(INDEX, 0);
  QModelIndex bottomRight = index(INDEX, 0);
  emit dataChanged(topLeft, bottomRight);
}

bool Board_graphic::is_check_mate() const
{
  return _is_check_mate;
}

void Board_graphic::set_check_mate()
{
  _is_check_mate = true;
  emit check_mate();
}

void Board_graphic::add_to_command_history(const QString& str)
{
  qDebug()<<"Board_graphic::add_to_command_history";
  _commands_history.append(str);
  emit commands_hist_changed();
}

void Board_graphic::path_to_file(QString &path, bool is_moves_from_file)
{
  for(int i = path.indexOf("/"); !path[path.indexOf("/", i) + 1].isLetter(); ++i)
    path.remove(0,i);

  if(is_moves_from_file)
    read_moves_from_file(path);
  else
  {
    path += "/chess_hist.txt";
    write_moves_to_file(path);
  }
}

void Board_graphic::write_moves_to_file(const QString& path)
{
  qDebug()<<"Board_graphic::write_moves_to_file";
  std::ofstream in_file(path.toStdString());
  for(auto &s : _str_moves_history)
  {
    in_file<<s.toStdString();
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void Board_graphic::read_moves_from_file(const QString& path)
{
  qDebug()<<"Board_graphic::read_move_from_file: "<<path;
  std::ifstream from_file(path.toStdString());
  std::string data_from_file(std::istream_iterator<char>(from_file), (std::istream_iterator<char>()));

  add_to_messages_for_server_stack(Messages::FROM_FILE, QString::fromStdString(data_from_file));
}

void Board_graphic::set_connect_status(const int status)
{
  qDebug()<<"Board_graphic::set_connect_status";
  switch(status)
  {
    case Messages::SERVER_HERE:
      if(_udp_connection_status == "Disconnected")
        _udp_connection_status = "Connect";
      break;
    case Messages::SERVER_LOST:
      _udp_connection_status = "Disconnected";
      break;
    case Messages::OPPONENT_LOST:
      _udp_connection_status = "Opponent disconnected";
      break;
    default:
      qDebug()<<"set_connect_status wrong: "<<status;
  }
  emit udp_connection_status_changed();
}

bool Board_graphic::is_new_message_for_server_appear() const
{
  //qDebug()<<"Board_graphic::is new command appear:"<<!_messages_for_server_stack.isEmpty();
  return !_messages_for_server_stack.isEmpty();
}

const QString Board_graphic::pull_first_messages_for_server()
{
  //qDebug()<<"Board_graphic::pull_first_command()";
  QString command(_messages_for_server_stack.first());
  _messages_for_server_stack.removeFirst();
  return command;
}

QStringList Board_graphic::get_moves_history() const
{
  return _str_moves_history;
}

QStringList Board_graphic::get_commands_hist() const
{
  return _commands_history;
}

int Board_graphic::get_last_elem_ind() const
{
  return _commands_history.size() - 1;
}

QString Board_graphic::get_move_turn_color() const
{
  return _move_color;
}

QString Board_graphic::get_udp_connection_status() const
{
  return _udp_connection_status;
}

void Board_graphic::addFigure(const Figure &figure)
{
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  _figures_model << figure;
  endInsertRows();
}

int Board_graphic::rowCount(const QModelIndex & parent) const
{
  Q_UNUSED(parent);
  return _figures_model.count();
}

QVariant Board_graphic::data(const QModelIndex & index, int role) const
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

QHash<int, QByteArray> Board_graphic::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[NameRole] = "figure_name";
  roles[XRole] = "x_coord";
  roles[YRole] = "y_coord";
  roles[VisibleRole] = "figure_visible";
  return roles;
}
