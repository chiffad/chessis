#include "logic/message_handler.hpp"

#include "messages/messages.hpp"

#include <boost/mpl/begin_end.hpp>
#include <spdlog/spdlog.h>
#include <typeinfo>

namespace logic {

namespace {
inline msg::game_inf_t get_board_state(const board_logic_t& d, const bool playing_white)
{
  return {d.get_board_mask(), d.get_moves_history(), d.mate(), static_cast<int>(d.get_move_num()), playing_white};
}

inline msg::inf_request_t get_person_inf(const player_t& player)
{
  return {"Login: " + player.credentials().login + "; Elo rating: " + std::to_string(player.rating())};
}

} // namespace

struct message_handler_t::impl_t
{
  impl_t(games_manager_t& games_manager, server::server_t& server)
    : games_manager_{games_manager}
    , server_{server}
  {}

  void process_mess_begin(const std::string& str, player_t& player) { process_mess<boost::mpl::begin<msg::message_types>::type>(str, player); }

  template<typename T>
  void process_mess(const std::string& str, player_t& player)
  {
    if (msg::is_equal_types<typename T::type>(str))
    {
      handle_fn<typename T::type>(str, player);
      return;
    }

    process_mess<typename boost::mpl::next<T>::type>(str, player);
  }

  template<msg::one_of_msg_types T>
  void handle_fn(const std::string& str, player_t& /*player*/)
  {
    SPDLOG_ERROR("For type={} tactic isn't defined! msg={}", typeid(T).name(), str);
  }

  void start_new_game(player_t& player)
  {
    const auto free_player_uuid = games_manager_.free_player(player);
    if (!free_player_uuid)
    {
      SPDLOG_INFO("Can not start game for cleint={}; not opponent found!", player);
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
    server_.send(get_board_state(board, player.playing_white()), player.address());

    const auto opp_uuid = games_manager_.opponent(player.uuid());
    if (!opp_uuid)
    {
      SPDLOG_INFO("No opponent found for player={}", player);
      return;
    }

    server_.send(get_board_state(board, !player.playing_white()), games_manager_.player(opp_uuid.value()).address());
  }

  games_manager_t& games_manager_;
  server::server_t& server_;
};

template<>
void message_handler_t::impl_t::process_mess<boost::mpl::end<msg::message_types>::type>(const std::string& str, player_t& player)
{
  SPDLOG_ERROR("No type found for str={}; player={};", str, player);
}

template<>
void message_handler_t::impl_t::handle_fn<msg::login_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for login_t; msg={}", message);
  auto login = msg::init<msg::login_t>(message);

  if (!player.credentials().login.empty() && player.credentials() != credentials_t{login.login, login.pwd})
  {
    SPDLOG_INFO("attempt to login to the player={} with incorrect credentials login={}; pwd={}", player, login.login, login.pwd);
    server_.send(msg::incorrect_log_t(), player.address());
    return;
  }

  player.set_credentials({login.login, login.pwd});
  // TODO check is there game in progress for this player update board?
  start_new_game(player);
}

template<>
void message_handler_t::impl_t::handle_fn<msg::opponent_inf_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for opponent_inf_t; msg={}", message);
  const auto opp_uuid = games_manager_.opponent(player.uuid());
  if (!opp_uuid)
  {
    server_.send(msg::inf_request_t("No opponent: no game in progress!"), player.address());
    return;
  }

  server_.send(get_person_inf(games_manager_.player(opp_uuid.value())), player.address());
}

template<>
void message_handler_t::impl_t::handle_fn<msg::my_inf_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for my_inf_t; msg={}", message);
  server_.send(get_person_inf(player), player.address());
}

template<>
void message_handler_t::impl_t::handle_fn<msg::client_lost_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for client_lost_t; msg={}", message);

  const auto opp_uuid = games_manager_.opponent(player.uuid());
  if (opp_uuid.has_value())
  {
    server_.send(msg::opponent_lost_t(), games_manager_.player(opp_uuid.value()).address());
  }
}

template<>
void message_handler_t::impl_t::handle_fn<msg::move_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for move_t; msg={}", message);

  const auto board_uuid = games_manager_.board(player);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  make_moves_from_str((msg::init<msg::move_t>(message)).data, games_manager_.board(board_uuid.value()));
  board_updated(player);
}

template<>
void message_handler_t::impl_t::handle_fn<msg::back_move_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for back_move_t; msg={}", message);

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
void message_handler_t::impl_t::handle_fn<msg::go_to_history_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for go_to_history_t; msg={}", message);

  const auto board_uuid = games_manager_.board(player);
  if (!board_uuid)
  {
    SPDLOG_ERROR("No board found for player={}", player);
    return;
  }

  games_manager_.board(board_uuid.value()).go_to_history(msg::init<msg::go_to_history_t>(message).index);
  board_updated(player);
}

template<>
void message_handler_t::impl_t::handle_fn<msg::new_game_t>(const std::string& message, player_t& player)
{
  SPDLOG_DEBUG("tactic for start_new_game_t; msg={}", message);
  start_new_game(player);
}

message_handler_t::message_handler_t(games_manager_t& games_manager, server::server_t& server)
  : impl_(std::make_unique<impl_t>(games_manager, server))
{}

message_handler_t::~message_handler_t() = default;

void message_handler_t::process_server_message(const endpoint_t& addr, const std::string& message)
{
  auto c = impl_->games_manager_.player(addr);
  // TODO: simplify!
  if (c)
  {
    impl_->process_mess_begin(message, *c.value());
    return;
  }

  SPDLOG_INFO("New player! addr={}", addr);
  const auto uuid = impl_->games_manager_.add_player(addr);
  impl_->process_mess_begin(message, impl_->games_manager_.player(uuid));
}

} // namespace logic