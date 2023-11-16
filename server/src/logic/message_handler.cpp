#include "logic/message_handler.hpp"

#include <messages/messages_io.hpp>

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <typeinfo>

namespace chess::logic {

namespace {

template<typename T>
concept one_of_msg_to_ignore = msg::one_of<T, msg::some_datagram_t, msg::message_received_t, msg::is_server_lost_t, msg::tokenized_msg_t, msg::login_t>;

inline msg::game_inf_t get_board_state(const board_logic_t& d, const bool playing_white)
{
  return {d.get_board_mask(), d.get_moves_history(), d.mate(), d.get_move_num(), playing_white};
}

} // namespace

struct message_handler_t::impl_t
{
  impl_t(games_manager_t& games_manager, server::server_t& server, const server::user_data::users_data_manager_t& users_data_manager)
    : games_manager_{games_manager}
    , server_{server}
    , users_data_manager_{users_data_manager}
  {}

  void process_mess_begin(const msg::some_datagram_t& datagram, player_t& player)
  {
    SPDLOG_TRACE("Begin processing of the datagram.type={}", datagram.type);
    process_mess<boost::mpl::begin<msg::to_server_messages_t>::type>(datagram, player);
  }

  // template<typename T> requires msg::one_of_to_server_msgs<typename T::type>
  template<typename T>
  void process_mess(const msg::some_datagram_t& datagram, player_t& player)
  {
    if constexpr (msg::one_of_to_server_msgs<typename T::type>)
    {
      if (datagram.type == msg::id_v<typename T::type>)
      {
        handle<typename T::type>(datagram, player);
        return;
      }
    }
    process_mess<typename boost::mpl::next<T>::type>(datagram, player);
  }

  template<msg::one_of_to_server_msgs T>
  void handle(const msg::some_datagram_t& datagram, player_t& /*player*/)
  {
    static_assert(one_of_msg_to_ignore<T>, "Unexpected type received. Handle it or ignore!");
    SPDLOG_ERROR("For type={} tactic isn't defined! datagram type={}", typeid(T).name(), datagram.type);
  }

  msg::inf_request_t get_person_inf(const player_t& player) const
  {
    return {"Login: " + users_data_manager_.credentials(player.uuid()).login + "; Elo rating: " + std::to_string(player.rating())};
  }

  void start_new_game(player_t& player)
  {
    const auto free_player_uuid = games_manager_.free_player(player);
    if (!free_player_uuid)
    {
      SPDLOG_INFO("Can not start game for cleint={}; no opponent found!", player);
      return;
    }

    games_manager_.start_game(player.uuid(), free_player_uuid.value());
    board_updated(player);
  }

  void board_updated(player_t& player)
  {
    const auto board_uuid = games_manager_.board(player);
    if (!board_uuid)
    {
      SPDLOG_ERROR("No board found for player={}", player);
      return;
    }

    const auto& board = games_manager_.board(board_uuid.value());
    server_.send(get_board_state(board, player.playing_white()), player.uuid());

    const auto opp_uuid = games_manager_.opponent(player.uuid());
    if (!opp_uuid)
    {
      SPDLOG_INFO("No opponent found for player={}", player);
      return;
    }

    server_.send(get_board_state(board, !player.playing_white()), opp_uuid.value());
  }

  games_manager_t& games_manager_;
  server::server_t& server_;
  const server::user_data::users_data_manager_t& users_data_manager_;
};

template<>
void message_handler_t::impl_t::process_mess<boost::mpl::end<msg::to_server_messages_t>::type>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_ERROR("No type found for datagram type={}; player={};", datagram.type, player);
}

template<>
void message_handler_t::impl_t::handle<msg::hello_server_t>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_DEBUG("tactic for hello_server_t;");

  // TODO check is there game in progress for this player update board
  // start_new_game(player);
}

template<>
void message_handler_t::impl_t::handle<msg::opponent_inf_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for opponent_inf_t;");
  const auto opp_uuid = games_manager_.opponent(player.uuid());
  if (!opp_uuid)
  {
    server_.send(msg::inf_request_t("No opponent: no game in progress!"), player.uuid());
    return;
  }

  server_.send(get_person_inf(games_manager_.player(opp_uuid.value())), player.uuid());
}

template<>
void message_handler_t::impl_t::handle<msg::my_inf_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for my_inf_t;");
  server_.send(get_person_inf(player), player.uuid());
}

template<>
void message_handler_t::impl_t::handle<msg::move_t>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_DEBUG("tactic for move_t; datagram={}", datagram.data);

  const auto board_uuid = games_manager_.board(player);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  make_moves_from_str((msg::init<msg::move_t>(datagram)).data, games_manager_.board(board_uuid.value()));
  board_updated(player);
}

template<>
void message_handler_t::impl_t::handle<msg::back_move_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for back_move_t;");

  const auto board_uuid = games_manager_.board(player);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  games_manager_.board(board_uuid.value()).back_move();
  board_updated(player);
}

template<>
void message_handler_t::impl_t::handle<msg::go_to_history_t>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_DEBUG("tactic for go_to_history_t; datagram={}", datagram.data);

  const auto board_uuid = games_manager_.board(player);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  games_manager_.board(board_uuid.value()).go_to_history(msg::init<msg::go_to_history_t>(datagram).index);
  board_updated(player);
}

template<>
void message_handler_t::impl_t::handle<msg::new_game_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for start_new_game_t;");
  start_new_game(player);
}

message_handler_t::message_handler_t(games_manager_t& games_manager, server::server_t& server, server::user_data::users_data_manager_t& users_data_manager)
  : impl_(std::make_unique<impl_t>(games_manager, server, users_data_manager))
{}

message_handler_t::~message_handler_t() = default;

void message_handler_t::process_server_message(const client_uuid_t& uuid, const msg::some_datagram_t& message)
try
{
  if (!impl_->games_manager_.count(uuid))
  {
    SPDLOG_ERROR("No client found for uuid={}", uuid);
    return;
  }

  impl_->process_mess_begin(message, impl_->games_manager_.player(uuid));
}
catch (const std::exception& ex)
{
  SPDLOG_CRITICAL("Exception during server message={} processing for user={}", message, uuid);
  throw;
}

void message_handler_t::user_connection_changed(const client_uuid_t& uuid, const bool online)
{
  SPDLOG_DEBUG("Client with uuid={} connection changed online={}", uuid, online);
  if (online) return;

  const auto opp_uuid = impl_->games_manager_.opponent(uuid);
  if (opp_uuid)
  {
    impl_->server_.send(msg::opponent_lost_t(), opp_uuid.value());
  }
}

void message_handler_t::user_connected(client_uuid_t uuid)
{
  SPDLOG_INFO("New player! uuid={}", uuid);
  impl_->games_manager_.add_player(std::move(uuid));
}

} // namespace chess::logic
