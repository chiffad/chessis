#include "logic/games_manager.hpp"

namespace logic {

games_manager_t::games_manager_t(io_service_t& io_service)
  : clients_{io_service}
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

server::clients_holder_t& games_manager_t::clients()
{
  return clients_;
}

server::client_t::uuid_t games_manager_t::add_client(const endpoint_t& addr)
{
  const auto uuid = clients_.add(addr);
  free_clients_.push_back(uuid);
  return uuid;
}

std::optional<server::client_t*> games_manager_t::client(const endpoint_t& addr)
{
  const auto it = clients_.find(addr);
  if (it == clients_.end()) return std::nullopt;

  return &it->second;
}

server::client_t& games_manager_t::client(const server::client_t::uuid_t& uuid)
{
  return clients_.at(uuid);
}

const server::client_t& games_manager_t::client(const server::client_t::uuid_t& uuid) const
{
  return clients_.at(uuid);
}

std::optional<server::client_t::uuid_t> games_manager_t::free_client(const server::client_t& not_this) const
{
  for (const auto& cl : free_clients_)
  {
    if (cl != not_this.uuid()) return cl;
  }

  return std::nullopt;
}

std::optional<board_logic_t::uuid_t> games_manager_t::board(const server::client_t& client) const
{
  const auto it = client_to_board_map_.find(client.uuid());
  if (it == client_to_board_map_.end()) return std::nullopt;

  return it->second;
}

void games_manager_t::start_game(const server::client_t::uuid_t& player_1, const server::client_t::uuid_t& player_2)
{
  clear_mapping(player_1);
  clear_mapping(player_2);

  const auto board_uuid = boards_.add();
  const bool white_player = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 2 == 0;
  clients_.at(player_1).set_playing_white(white_player);
  clients_.at(player_2).set_playing_white(!white_player);

  // TODO: remove player_1 and player_2 from free_clients_;

  add_mapping(player_1, player_2, board_uuid);
}

std::optional<server::client_t::uuid_t> games_manager_t::opponent(const server::client_t::uuid_t& client_uuid) const
{
  const auto client_2_board_it = client_to_board_map_.find(client_uuid);
  if (client_2_board_it == client_to_board_map_.end()) return std::nullopt;

  const auto board_2_client_it = board_to_clients_map_.find(client_2_board_it->second);
  if (board_2_client_it == board_to_clients_map_.end()) return std::nullopt;

  for (const auto& el : board_2_client_it->second)
  {
    if (el != client_uuid) return el;
  }

  return std::nullopt;
}

void games_manager_t::add_mapping(const server::client_t::uuid_t& player_1, const server::client_t::uuid_t& player_2, const board_logic_t::uuid_t& board_uuid)
{
  client_to_board_map_[player_1] = board_uuid;
  client_to_board_map_[player_2] = board_uuid;
  board_to_clients_map_[board_uuid] = {player_1, player_2};
}

void games_manager_t::clear_mapping(const server::client_t::uuid_t& client)
{
  const auto client_2_board_it = client_to_board_map_.find(client);
  if (client_2_board_it == client_to_board_map_.end()) return;

  const auto board_2_client_it = board_to_clients_map_.find(client_2_board_it->second);
  client_to_board_map_.erase(client_2_board_it);

  if (board_2_client_it == board_to_clients_map_.end()) return;

  board_2_client_it->second.erase(client);
  if (board_2_client_it->second.empty())
  {
    boards_.erase(client_2_board_it->second);
    board_to_clients_map_.erase(board_2_client_it);
  }
}

} // namespace logic