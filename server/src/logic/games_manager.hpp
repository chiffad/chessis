#pragma once

#include "common/helper.hpp"
#include "logic/boards_holder.hpp"
#include "server/clients_holder.hpp"

#include <list>
#include <map>
#include <optional>
#include <set>

namespace logic {

class games_manager_t
{
public:
  explicit games_manager_t(io_service_t& io_service);
  board_logic_t& board(const board_logic_t::uuid_t& uuid);
  const board_logic_t& board(const board_logic_t::uuid_t& uuid) const;

  server::clients_holder_t& clients();
  server::client_t::uuid_t add_client(const endpoint_t& addr);
  std::optional<server::client_t*> client(const endpoint_t& addr);
  server::client_t& client(const server::client_t::uuid_t& uuid);
  const server::client_t& client(const server::client_t::uuid_t& uuid) const;

  void start_game(const server::client_t::uuid_t& player_1, const server::client_t::uuid_t& player_2);
  std::optional<server::client_t::uuid_t> opponent(const server::client_t::uuid_t& client) const;
  std::optional<server::client_t::uuid_t> free_client(const server::client_t& not_this) const;
  std::optional<board_logic_t::uuid_t> board(const server::client_t& client) const;

private:
  void add_mapping(const server::client_t::uuid_t& player_1, const server::client_t::uuid_t& player_2, const board_logic_t::uuid_t& board_uuid);
  void clear_mapping(const server::client_t::uuid_t& client);

private:
  server::clients_holder_t clients_;
  boards_holder_t boards_;

  std::list<server::client_t::uuid_t> free_clients_;
  std::map<board_logic_t::uuid_t, std::set<server::client_t::uuid_t>> board_to_clients_map_;
  std::map<server::client_t::uuid_t, board_logic_t::uuid_t> client_to_board_map_;
};

} // namespace logic