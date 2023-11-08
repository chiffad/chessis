#pragma once

#include "messages/messages.hpp"
#include "spdlog/fmt/ostr.h"

namespace chess::msg {

inline std::ostream& operator<<(std::ostream& os, const token_t& el)
{
  return os << "token_t{ uuid=" << el.uuid << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const login_t& el)
{
  return os << "login_t{ login=" << el.login << "; pwd=" << el.pwd << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const move_t& el)
{
  return os << "move_t{ data=" << el.data << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const go_to_history_t& el)
{
  return os << "go_to_history_t{ index=" << el.index << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const inf_request_t& el)
{
  return os << "inf_request_t{ data=" << el.data << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const game_inf_t& el)
{
  return os << "game_inf_t{ board_mask=" << el.board_mask << "; moves_history=" << el.moves_history << "; is_mate=" << el.is_mate << "; move_num=" << el.move_num
            << "; playing_white=" << el.playing_white << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const login_response_t& el)
{
  return os << "login_response_t{ logic_server_address=" << el.logic_server_address << "; logic_server_port=" << el.logic_server_port << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const some_datagram_t& el)
{
  return os << "some_datagram_t{ data=" << el.data << "; type=" << el.type << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const incoming_datagram_t& el)
{
  return os << "incoming_datagram_t{ data=" << el.data << "; ser_num=" << el.ser_num << "; response_ser_num=" << el.response_ser_num << "; }";
}

inline std::ostream& operator<<(std::ostream& os, const tokenized_msg_t& el)
{
  return os << "tokenized_msg_t{ token=" << el.token << "; data=" << el.data << "; }";
}

#define GEN_PRINT_FN(name)                                                                                                                                                         \
  inline std::ostream& operator<<(std::ostream& os, const name& el)                                                                                                                \
  {                                                                                                                                                                                \
    return os << #name << "{}";                                                                                                                                                    \
  }
GEN_PRINT_FN(hello_server_t);
GEN_PRINT_FN(message_received_t);
GEN_PRINT_FN(is_server_lost_t);
GEN_PRINT_FN(is_client_lost_t);
GEN_PRINT_FN(get_login_t);
GEN_PRINT_FN(incorrect_log_t);
GEN_PRINT_FN(opponent_lost_t);
GEN_PRINT_FN(opponent_inf_t);
GEN_PRINT_FN(my_inf_t);
GEN_PRINT_FN(back_move_t);
GEN_PRINT_FN(new_game_t);
#undef GEN_PRINT_FN

} // namespace chess::msg
