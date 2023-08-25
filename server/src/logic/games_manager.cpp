#include "logic/games_manager.hpp"

namespace logic {

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

player_t::uuid_t games_manager_t::add_player(const endpoint_t& addr)
{
  const auto uuid = players_.add(addr);
  free_players_.push_back(uuid);
  return uuid;
}

std::optional<player_t*> games_manager_t::player(const endpoint_t& addr)
{
  const auto it = players_.find(addr);
  if (it == players_.end()) return std::nullopt;

  return &it->second;
}

player_t& games_manager_t::player(const player_t::uuid_t& uuid)
{
  return players_.at(uuid);
}

const player_t& games_manager_t::player(const player_t::uuid_t& uuid) const
{
  return players_.at(uuid);
}

std::optional<player_t::uuid_t> games_manager_t::free_player(const player_t& not_this) const
{
  for (const auto& cl : free_players_)
  {
    if (cl != not_this.uuid()) return cl;
  }

  return std::nullopt;
}

std::optional<board_logic_t::uuid_t> games_manager_t::board(const player_t& player) const
{
  const auto it = player_to_board_map_.find(player.uuid());
  if (it == player_to_board_map_.end()) return std::nullopt;

  return it->second;
}

void games_manager_t::start_game(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2)
{
  clear_mapping(player_1);
  clear_mapping(player_2);

  const auto board_uuid = boards_.add();
  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  players_.at(player_1).set_playing_white(white_player);
  players_.at(player_2).set_playing_white(!white_player);

  // TODO: remove player_1 and player_2 from free_players_;

  add_mapping(player_1, player_2, board_uuid);
}

std::optional<player_t::uuid_t> games_manager_t::opponent(const player_t::uuid_t& player_uuid) const
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

void games_manager_t::add_mapping(const player_t::uuid_t& player_1, const player_t::uuid_t& player_2, const board_logic_t::uuid_t& board_uuid)
{
  player_to_board_map_[player_1] = board_uuid;
  player_to_board_map_[player_2] = board_uuid;
  board_to_player_map_[board_uuid] = {player_1, player_2};
}

void games_manager_t::clear_mapping(const player_t::uuid_t& player)
{
  const auto player_2_board_it = player_to_board_map_.find(player);
  if (player_2_board_it == player_to_board_map_.end()) return;

  const auto board_2_player_it = board_to_player_map_.find(player_2_board_it->second);
  player_to_board_map_.erase(player_2_board_it);

  if (board_2_player_it == board_to_player_map_.end()) return;

  board_2_player_it->second.erase(player);
  if (board_2_player_it->second.empty())
  {
    boards_.erase(player_2_board_it->second);
    board_to_player_map_.erase(board_2_player_it);
  }
}

std::vector<server::datagram_t> games_manager_t::messages_to_send()
{
  std::vector<server::datagram_t> res;
  for (auto& player : players_)
  {
    while (player.second.message_for_server_append())
    {
      res.push_back(server::datagram_t{player.second.address(), player.second.pull_for_server()});
    }
  }
  return res;
}

} // namespace logic