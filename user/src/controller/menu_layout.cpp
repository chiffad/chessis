#include "controller/menu_layout.hpp"

#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>

#include "helper.h"

namespace {
constexpr const char* const MOVE_COLOR_W = "img/w_k.png";
constexpr const char* const MOVE_COLOR_B = "img/b_K.png";
const char FREE_SPACE = ' ';
} // namespace

namespace controller {

menu_layout_t::menu_layout_t(const command_requested_callback_t& callback)
  : QObject()
  , move_color_(MOVE_COLOR_W)
  , connection_status_("Disconnected")
  , command_requested_callback_(callback)
{}

menu_layout_t::~menu_layout_t() = default;

void menu_layout_t::run_command(const QString& message, const int x)
{
  const QString HELP_WORD = "help";
  const QString BACK_MOVE = "back";
  const QString MOVE_WORD = "move";
  const QString SHOW_ME = "show me";
  const QString NEW_GAME = "new game";
  const QString HISTORY = "to history";
  const QString SHOW_OPPONENT = "show opponent";

  SPDLOG_DEBUG("run command={}; x={}", message, x);

  if (message == HELP_WORD)
  {
    SPDLOG_DEBUG("run comman=dhelp_word");
    add_to_command_history("1.For move, type '" + MOVE_WORD + "' and coordinates(example: " + MOVE_WORD + " d2-d4)" + "\n" + "2.For back move, type '" +
                           BACK_MOVE + "'" + "\n" + "3.For start new game, type '" + NEW_GAME + "'" + "\n" + "4.For go to history index, type '" + HISTORY +
                           "' and index" + "\n" + "5.To view opponent information, print '" + SHOW_OPPONENT + "'" + "\n" +
                           "6.To view your information, print '" + SHOW_ME + "'");
  }
  else if (message == SHOW_OPPONENT)
  {
    command_requested(msg::opponent_inf_t());
  }
  else if (message == SHOW_ME)
  {
    command_requested(msg::my_inf_t());
  }
  else if (message == NEW_GAME)
  {
    command_requested(msg::new_game_t());
  }
  else if (message == BACK_MOVE)
  {
    command_requested(msg::back_move_t());
  }
  else if (message == HISTORY)
  {
    msg::go_to_history_t gth;
    gth.index = x + 1;
    command_requested(gth);
    add_to_command_history("command: " + message + " " + QString::number(x + 1));
    return;
  }
  else if (message.contains(HISTORY))
  {
    msg::go_to_history_t gth;
    gth.index = message.mid(HISTORY.size() + 1).toInt();
    command_requested(gth);
  }
  else add_to_command_history("Unknown command (type '" + HELP_WORD + "' for help).");

  add_to_command_history("command: " + message);
}

void menu_layout_t::set_moves_history(const QString& history)
{
  str_moves_history_.clear();

  const int NEED_SIMB_TO_MOVE = 4;

  QString move;
  for (const auto& simb : history)
  {
    move.append(simb);

    if (move.size() == NEED_SIMB_TO_MOVE)
    {
      move.insert(2, " - ");
      str_moves_history_.append(move);
      move.clear();
    }
  }

  emit moves_history_changed();
}

void menu_layout_t::set_move_color(const int move_num)
{
  move_color_ = (move_num % 2 == 0) ? MOVE_COLOR_W : MOVE_COLOR_B;
  emit move_turn_color_changed();
}

void menu_layout_t::add_to_command_history(const QString& str)
{
  commands_history_.append(str);
  emit commands_hist_changed();
}

void menu_layout_t::path_to_file(QString& path, bool is_moves_from_file)
{
  for (int i = path.indexOf("/"); !path[path.indexOf("/", i) + 1].isLetter(); ++i)
  {
    path.remove(0, i);
  }

  if (is_moves_from_file)
  {
    read_moves_from_file(path);
  }
  else
  {
    path += "/chess_hist.txt";
    write_moves_to_file(path);
  }
}

void menu_layout_t::write_moves_to_file(const QString& path)
{
  std::ofstream in_file(path.toStdString());
  if (!in_file.is_open())
  {
    SPDLOG_ERROR("Couldn't open file.");
    return;
  }
  for (auto& s : str_moves_history_)
  {
    in_file << s.toStdString();
    in_file << FREE_SPACE;
  }
  in_file.close();
}

void menu_layout_t::read_moves_from_file(const QString& path)
{
  std::ifstream from_file(path.toStdString());
  if (!from_file.is_open())
  {
    SPDLOG_ERROR("Couldn't open file={}", path);
    return;
  }

  std::string data_from_file(std::istream_iterator<char>(from_file), (std::istream_iterator<char>()));

  command_requested(msg::move_t(data_from_file));
}

void menu_layout_t::set_connect_status(const int status)
{
  switch (status)
  {
    case msg::id<msg::server_here_t>():
      if (connection_status_ == "Disconnected")
      {
        connection_status_ = "Connect";
      }
      break;
    case msg::id<msg::server_lost_t>(): connection_status_ = "Disconnected"; break;
    case msg::id<msg::opponent_lost_t>(): connection_status_ = "Opponent disconnected"; break;
    default: SPDLOG_ERROR("Unknown status={}", status); return;
  }
  emit connection_status_changed();
}

QStringList menu_layout_t::moves_history() const
{
  return str_moves_history_;
}

QStringList menu_layout_t::commands_hist() const
{
  return commands_history_;
}

int menu_layout_t::last_command_hist_ind() const
{
  return commands_history_.size() - 1;
}

QString menu_layout_t::move_turn_color() const
{
  return move_color_;
}

QString menu_layout_t::connection_status() const
{
  return connection_status_;
}

} // namespace controller