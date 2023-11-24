#include "logic/message_handler.hpp"

#include <messages/messages_io.hpp>

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <typeinfo>

namespace chess::logic {

namespace {

template<typename T>
concept one_of_msg_to_ignore = msg::one_of<T, msg::some_datagram_t, msg::message_received_t, msg::is_server_lost_t, msg::tokenized_msg_t, msg::login_t, msg::hello_server_t>;

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
    const auto& user = users_data_manager_.user(player.uuid);
    return {"Login: " + user.credentials().login + "; Elo rating: " + std::to_string(user.elo_rating())};
  }

  void start_new_game(player_t& player)
  {
    const auto free_player_uuid = games_manager_.free_player_uuid(player.uuid);
    if (!free_player_uuid)
    {
      SPDLOG_INFO("Can not start game for cleint={}; no opponent found!", player);
      return;
    }

    const auto board_uuid = games_manager_.start_game(player.uuid, free_player_uuid.value());
    board_updated(games_manager_.board(board_uuid), player);
  }

  void send_board_state(const board_logic_t& board, const player_t& player)
  {
    msg::game_inf_t board_state = get_board_state(board, player.playing_white);
    server_.send(board_state, player.uuid);
  }

  void board_updated(const board_logic_t& board, const player_t& player)
  {
    const auto opp_uuid = games_manager_.opponent_uuid(player.uuid);
    if (!opp_uuid)
    {
      SPDLOG_ERROR("No opponent found for player={}", player);
      return;
    }

    send_board_state(board, player);
    send_board_state(board, games_manager_.player(opp_uuid.value()));
  }

  void exec_on_board_and_send_update(const board_logic_t::uuid_t& board_uuid, player_t& player, const std::function<void(board_logic_t&)>& extra_logic)
  {
    auto& board = games_manager_.board(board_uuid);
    extra_logic(board);
    board_updated(board, player);
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
void message_handler_t::impl_t::handle<msg::opponent_inf_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for opponent_inf_t;");
  const auto opp_uuid = games_manager_.opponent_uuid(player.uuid);
  if (!opp_uuid)
  {
    server_.send(msg::inf_request_t("No opponent: no game in progress!"), player.uuid);
    return;
  }

  server_.send(get_person_inf(games_manager_.player(opp_uuid.value())), player.uuid);
}

template<>
void message_handler_t::impl_t::handle<msg::my_inf_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for my_inf_t;");
  server_.send(get_person_inf(player), player.uuid);
}

template<>
void message_handler_t::impl_t::handle<msg::move_t>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_DEBUG("tactic for move_t; datagram={}", datagram.data);

  const auto board_uuid = games_manager_.board_uuid(player.uuid);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  exec_on_board_and_send_update(board_uuid.value(), player, [&](board_logic_t& board) { make_moves_from_str((msg::init<msg::move_t>(datagram)).data, board); });
}

template<>
void message_handler_t::impl_t::handle<msg::back_move_t>(const msg::some_datagram_t& /*datagram*/, player_t& player)
{
  SPDLOG_DEBUG("tactic for back_move_t;");

  const auto board_uuid = games_manager_.board_uuid(player.uuid);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  exec_on_board_and_send_update(board_uuid.value(), player, [](board_logic_t& board) { board.back_move(); });
}

template<>
void message_handler_t::impl_t::handle<msg::go_to_history_t>(const msg::some_datagram_t& datagram, player_t& player)
{
  SPDLOG_DEBUG("tactic for go_to_history_t; datagram={}", datagram.data);

  const auto board_uuid = games_manager_.board_uuid(player.uuid);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  exec_on_board_and_send_update(board_uuid.value(), player, [&](board_logic_t& board) { board.go_to_history(msg::init<msg::go_to_history_t>(datagram).index); });
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
  if (online)
  { // i.e. connected or reconnected
    const auto& player = impl_->games_manager_.add_player(std::move(uuid));

    auto board_uuid_opt = impl_->games_manager_.board_uuid(player.uuid);
    if (!board_uuid_opt) return;

    SPDLOG_INFO("found board={} for player={}", board_uuid_opt.value(), player.uuid);
    impl_->send_board_state(impl_->games_manager_.board(board_uuid_opt.value()), player);
  }

  const auto opp_uuid = impl_->games_manager_.opponent_uuid(uuid);
  if (opp_uuid)
  {
    if (online) impl_->server_.send(msg::opponent_online_t(), opp_uuid.value());
    else impl_->server_.send(msg::opponent_lost_t(), opp_uuid.value());
  }
}

} // namespace chess::logic
