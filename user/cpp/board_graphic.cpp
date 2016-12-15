#include "board_graphic.h"

#include <QPainter>
#include <ctype.h>
#include <QModelIndex>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <stdlib.h>
#include <time.h>


using namespace graphic;

Board_graphic::Board_graphic() 
    : QAbstractListModel(nullptr), _move_color(MOVE_COLOR_W),
      _udp_connection_status("Disconnected"), _is_check_mate(false)
{
  enum {HILIGHT_CELLS = 2, FIGURES_NUMBER = 32};
  const QString HILIGHT_IM = "hilight";

  for(int i = 0; i < FIGURES_NUMBER + HILIGHT_CELLS; ++i)
    { addFigure(Figure(HILIGHT_IM, 0, 0, false)); }

//  srand (time(NULL));
//  set_login("asdasd" + QString::number(rand()));
}

Board_graphic::~Board_graphic()
{
}

void Board_graphic::run_command(const QString& message, const int x1, const int y1, const int x2, const int y2)
{
  const QString HELP_WORD = "help";
  const QString MOVE_WORD = "move";
  const QString BACK_MOVE = "back";
  const QString SHOW_ME = "show me";
  const QString NEW_GAME = "new game";
  const QString HISTORY = "to history";
  const QString SHOW_OPPONENT = "show opponent";

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
    { add_to_messages_for_server(Messages::OPPONENT_INF); }
  else if(message == SHOW_ME)
    { add_to_messages_for_server(Messages::MY_INF); }
  else if(message == NEW_GAME)
    { add_to_messages_for_server(Messages::NEW_GAME); }
  else if(message == BACK_MOVE)
    { add_to_messages_for_server(Messages::BACK_MOVE); }
  else if(message == HISTORY)
    { add_to_messages_for_server(Messages::GO_TO_HISTORY, QString::number(x1 + 1)); }
  else
  {
    const QString command(message.mid(0,message.indexOf(FREE_SPACE)));
    const QString command_content(message.mid(command.size()));

    if(command == MOVE_WORD)
    {
      if(!command_content.isEmpty())
        { add_to_messages_for_server(Messages::MOVE, command_content); }
      else
      {
        Coord from, to;
        if(set_correct_coord(from, x1, y1) && set_correct_coord(to, x2, y2))
          { add_to_messages_for_server(Messages::MOVE, coord_to_str(from, to)); }
      }
    }
    else add_to_command_history("Unknown command (type '" + HELP_WORD + "' for help).");
  }
}

bool Board_graphic::set_correct_coord(Coord& c, const int x, const int y)
{
  if(x < 0 || y < 0 || x > (CELL_SIZE_X * CELL_NUM) || y > (CELL_SIZE_Y * CELL_NUM))
  {
    qDebug()<<"Warning! in Board_graphic::set_correct_coord: Incorrect coord";
    return false;
  }

  c.x = (x + (CELL_SIZE_X / 2)) / CELL_SIZE_X;
  c.y = (y + (CELL_SIZE_Y / 2)) / CELL_SIZE_Y;

  return true;
}

void Board_graphic::update_coordinates()
{
  const auto FREE_FIELD = '.';

  auto f_it = _field.begin();
  for(auto &fig_mod : _figures_model)
  {
    f_it = std::find_if(f_it, _field.end(), [&FREE_FIELD](auto const &i) {return i != FREE_FIELD;});
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

Coord Board_graphic::get_field_coord(const int i) const
{
  return Coord (i % CELL_NUM, i / CELL_NUM);
}

void Board_graphic::set_board_mask(const QString& mask)
{
  if(mask.size() != CELL_NUM * CELL_NUM)
  {
    qDebug()<<"Warning! in Board_graphic::set_board_mask: Wrong board mask size";
    return;
  }

  _field.clear();
  _field = mask;

  update_coordinates();
}

void Board_graphic::set_moves_history(const QString& history)
{
  _str_moves_history.clear();

  const int NEED_SIMB_TO_MOVE = 4;

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

  emit moves_history_changed();
}

void Board_graphic::set_move_color(const int move_num)
{
  _move_color = (move_num % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

const QString Board_graphic::coord_to_str(const Coord &from, const Coord &to) const
{
  return (QChar(a_LETTER + from.x) + QString::number(CELL_NUM - from.y)
          + " - " + QChar(a_LETTER + to.x) + QString::number(CELL_NUM - to.y));
}

void Board_graphic::add_to_messages_for_server(const Messages::MESSAGE mes_type, const QString& content)
{
  _messages_for_server.append(QString::number(mes_type) + FREE_SPACE + content);
}

void Board_graphic::update_hilight(const int move_num, const QString& history)
{
  enum HILIGHT {FIRST_HILIGHT = 32, SECOND_HILIGHT = 33};

  const int CHAR_IN_MOVE = 4;
  if(move_num && history.size() >= move_num * CHAR_IN_MOVE)
  {
    auto simb = history.begin();
    simb += (move_num-1) * CHAR_IN_MOVE;
    Coord c;
    for(int i = 0; i < 2; ++i)
    {
      c.x = (*(simb++)).unicode() - a_LETTER;
      c.y = CELL_NUM - (*(simb++)).digitValue();
      HILIGHT ind = (i == 0) ? SECOND_HILIGHT : FIRST_HILIGHT;
      _figures_model[ind].set_visible(true);
      _figures_model[ind].set_coord(c);
      emit_figure_changed(ind);
    }
  }
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
  _commands_history.append(str);
  emit commands_hist_changed();
}

void Board_graphic::path_to_file(QString &path, bool is_moves_from_file)
{
  for(int i = path.indexOf("/"); !path[path.indexOf("/", i) + 1].isLetter(); ++i)
    { path.remove(0,i); }

  if(is_moves_from_file)
    { read_moves_from_file(path); }
  else
  {
    path += "/chess_hist.txt";
    write_moves_to_file(path);
  }
}

void Board_graphic::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toStdString());
  if(!in_file.is_open())
  {
    qDebug()<<"Warning! in Board_graphic::write_moves_to_file: Couldn't open file.";
    return;
  }
  for(auto &s : _str_moves_history)
  {
    in_file<<s.toStdString();
    in_file<<FREE_SPACE;
  }
  in_file.close();
}

void Board_graphic::read_moves_from_file(const QString& path)
{
  std::ifstream from_file(path.toStdString());
  if(!from_file.is_open())
  {
    qDebug()<<"Warning! Board_graphic::write_moves_to_file: Couldn't open file.";
    return;
  }

  std::string data_from_file(std::istream_iterator<char>(from_file), (std::istream_iterator<char>()));

  add_to_messages_for_server(Messages::FROM_FILE, QString::fromStdString(data_from_file));
}

void Board_graphic::set_connect_status(const int status)
{
  switch(status)
  {
    case Messages::SERVER_HERE:
      if(_udp_connection_status == "Disconnected")
        { _udp_connection_status = "Connect"; }
      break;
    case Messages::SERVER_LOST:
      _udp_connection_status = "Disconnected";
      break;
    case Messages::OPPONENT_LOST:
      _udp_connection_status = "Opponent disconnected";
      break;
    default:
      qDebug()<<"Warning! in Board_graphic::set_connect_status: Unknown status";
      return;
  }
  emit udp_connection_status_changed();
}

bool Board_graphic::set_login(const QString &login)
{
  for(auto i : login)
  {
    if(!i.isLetterOrNumber())
      { return false; }
  }

  add_to_messages_for_server(Messages::HELLO_SERVER, login);
  return true;
}

bool Board_graphic::is_message_appear() const
{
  return !_messages_for_server.isEmpty();
}

const QString Board_graphic::pull()
{
  QString command(_messages_for_server.first());
  _messages_for_server.removeFirst();
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

int Board_graphic::get_last_command_hist_ind() const
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
    { return QVariant(); }

  const Figure &figure = _figures_model[index.row()];
  if (role == NameRole)
    { return figure.name(); }
  else if (role == XRole)
    { return figure.x(); }
  else if (role == YRole)
    { return figure.y(); }
  else if (role == VisibleRole)
    { return figure.visible(); }
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
