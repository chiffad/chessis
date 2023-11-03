#pragma once

#include "messages/messages.hpp"

namespace chess::msg {

inline bool operator==(const token_t& el1, const token_t& el2)
{
  return std::tie(el1.uuid) == std::tie(el1.uuid);
}

inline bool operator==(const login_t& el1, const login_t& el2)
{
  return std::tie(el1.login, el1.pwd) == std::tie(el2.login, el2.pwd);
}

inline bool operator==(const move_t& el1, const move_t& el2)
{
  return std::tie(el1.data) == std::tie(el2.data);
}

inline bool operator==(const go_to_history_t& el1, const go_to_history_t& el2)
{
  return std::tie(el1.index) == std::tie(el2.index);
}

inline bool operator==(const inf_request_t& el1, const inf_request_t& el2)
{
  return std::tie(el1.data) == std::tie(el2.data);
}

inline bool operator==(const game_inf_t& el1, const game_inf_t& el2)
{
  return std::tie(el1.board_mask, el1.moves_history, el1.is_mate, el1.move_num, el1.playing_white) ==
         std::tie(el2.board_mask, el2.moves_history, el2.is_mate, el2.move_num, el2.playing_white);
}

inline bool operator==(const login_response_t& el1, const login_response_t& el2)
{
  return std::tie(el1.logic_server_address, el1.logic_server_port) == std::tie(el2.logic_server_address, el2.logic_server_port);
}

inline bool operator==(const some_datagram_t& el1, const some_datagram_t& el2)
{
  return std::tie(el1.type, el1.data) == std::tie(el2.type, el2.data);
}

inline bool operator==(const incoming_datagram_t& el1, const incoming_datagram_t& el2)
{
  return std::tie(el1.ser_num, el1.data, el2.response_ser_num) == std::tie(el2.ser_num, el2.data, el2.response_ser_num);
}

inline bool operator==(const tokenized_msg_t& el1, const tokenized_msg_t& el2)
{
  return std::tie(el1.token, el1.data) == std::tie(el2.token, el2.data);
}

} // namespace chess::msg
