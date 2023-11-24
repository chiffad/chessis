#include "logic/games_manager.hpp"

#include <spdlog/spdlog.h>

namespace chess::logic {

games_manager_t::games_manager_t(io_service_t& io_service)
  : players_{}
  , boards_{}
{}

board_logic_t& games_manager_t::board(const board_logic_t::uuid_t& uuid)
{
  return boards_.at(uuid);
}

const board_logic_t& games_manager_t::board(const board_logic_t::uuid_t& uuid) const
{
  return boards_.at(uuid);
}

player_t& games_manager_t::add_player(const client_uuid_t& uuid)
{
  const auto add_res = players_.add(uuid);
  if (add_res.second)
  {
    // TODO: add player to free player only when it press start game
    free_players_.insert(uuid);
  }
  return add_res.first->second;
}

bool games_manager_t::count(const client_uuid_t& uuid) const
{
  return players_.count(uuid);
}

player_t& games_manager_t::player(const client_uuid_t& uuid)
{
  return players_.at(uuid);
}

const player_t& games_manager_t::player(const client_uuid_t& uuid) const
{
  return players_.at(uuid);
}

std::optional<client_uuid_t> games_manager_t::free_player_uuid(const client_uuid_t& not_this) const
{
  SPDLOG_DEBUG("Free players number={}", free_players_.size());
  for (const auto& cl : free_players_)
  {
    if (cl != not_this) return cl;
  }

  return std::nullopt;
}

std::optional<board_logic_t::uuid_t> games_manager_t::board_uuid(const client_uuid_t& player_uuid) const
{
  const auto it = player_to_board_map_.find(player_uuid);
  if (it == player_to_board_map_.end()) return std::nullopt;

  return it->second;
}

board_logic_t::uuid_t games_manager_t::start_game(const client_uuid_t& player_1, const client_uuid_t& player_2)
{
  const auto board_uuid = boards_.add(); // TODO: check where we clean boards?
  SPDLOG_INFO("Start game beween player={} and player={}; board={}", player_1, player_2, board_uuid);

  unbook_player(player_1);
  unbook_player(player_2);
  book_players(player_1, player_2, board_uuid);

  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  players_.at(player_1).playing_white = white_player;
  players_.at(player_2).playing_white = !white_player;

  return board_uuid;
}

std::optional<client_uuid_t> games_manager_t::opponent_uuid(const client_uuid_t& player_uuid) const
{
  const auto player_2_board_it = player_to_board_map_.find(player_uuid);
  if (player_2_board_it == player_to_board_map_.end()) return std::nullopt;

  const auto board_2_player_it = board_to_player_map_.find(player_2_board_it->second);
  if (board_2_player_it == board_to_player_map_.end()) return std::nullopt;

  for (const auto& el : board_2_player_it->second)
  {
    if (el != player_uuid) return el;
  }

  return std::nullopt;
}

void games_manager_t::book_players(const client_uuid_t& player_1, const client_uuid_t& player_2, const board_logic_t::uuid_t& board_uuid)
{
  if (!free_players_.count(player_1) || !free_players_.count(player_2))
  {
    SPDLOG_ERROR("Can not book players: {} and {} as some of them already booked!", player_1, player_2);
    throw std::logic_error("Can not book already booked players!");
  }

  player_to_board_map_[player_1] = board_uuid;
  player_to_board_map_[player_2] = board_uuid;
  board_to_player_map_[board_uuid] = {player_1, player_2};
  free_players_.erase(player_1);
  free_players_.erase(player_2);
}

void games_manager_t::unbook_player(const client_uuid_t& player_uuid)
{
  const auto player_2_board_it = player_to_board_map_.find(player_uuid);
  if (player_2_board_it == player_to_board_map_.end()) return;

  free_players_.insert(player_uuid);

  const auto board_2_player_it = board_to_player_map_.find(player_2_board_it->second);
  player_to_board_map_.erase(player_2_board_it);

  if (board_2_player_it == board_to_player_map_.end()) return;

  board_2_player_it->second.erase(player_uuid);
  if (board_2_player_it->second.empty())
  {
    boards_.erase(player_2_board_it->second);
    board_to_player_map_.erase(board_2_player_it);
  }
}

} // namespace chess::logic
