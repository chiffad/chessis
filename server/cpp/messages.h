#ifndef __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
#define __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD

#include <string>
#include <memory>


namespace messages
{
  enum MESSAGE {WRONG_TYPE, HELLO_SERVER, MESSAGE_RECEIVED, IS_SERVER_LOST, IS_CLIENT_LOST,
                OPPONENT_INF, MY_INF, GET_LOGIN, LOGIN, INCORRECT_LOG,
                MOVE, BACK_MOVE, GO_TO_HISTORY, NEW_GAME, INF_REQUEST,
                SERVER_LOST, SERVER_HERE, CLIENT_LOST, OPPONENT_LOST, GAME_INF};

  MESSAGE cut_type(std::string& message);

  namespace detail
  {
    struct mess_t
    {
      virtual ~mess_t();
      bool is_ok = true;

      virtual std::string to_json() const = 0;
      virtual void from_json(const std::string& str) = 0;
    };
  }
/*
  struct hello_server_t // + server get
  {
  };

  struct message_received_t // + server get
  {
  };

  struct is_server_lost_t // + server get
  {
  };

  struct is_client_lost_t //  + client_get
  {
  };

  struct opponent_inf_t // + server get
  {
  };

  struct my_inf_t //+ server get
  {
  };

  struct get_login_t //+ server get
  {
  };
*/
  struct login_t : public detail::mess_t//+ server get
  {
    login_t();
    std::string login;
//    std::string pwd;

    virtual std::string to_json() const;
    virtual void from_json(const std::string& str);
  };

/*  struct incorrect_log_t //+ server get
  {
  };
*/
  struct move_t : public detail::mess_t//+ server get
  {
    move_t();
    std::string data;

    virtual std::string to_json() const;
    virtual void from_json(const std::string& str);
  };

/*  struct back_move_t //+ server get
  {
  };
*/
  struct go_to_history_t : public detail::mess_t//+ server get
  {
    go_to_history_t();
    int hist_ind;

    virtual std::string to_json() const;
    virtual void from_json(const std::string& str);
  };
/*
  struct new_game_t//+ server get
  {
  };
*/
  struct inf_request_t : public detail::mess_t// + client get
  {
    inf_request_t();
    std::string data;

    virtual std::string to_json() const;
    virtual void from_json(const std::string& str);
  };
/*
  struct server_lost_t // + client get
  {
  };

  struct server_here_t // + client get
  {
  };

  struct client_lost_t //+ server get
  {
  };

  struct opponent_lost_t// + client get
  {
  };
*/
  struct game_inf_t : public detail::mess_t// + client get
  {
    game_inf_t();
    std::string board_mask;
    std::string moves_history;
    bool is_mate;
    int move_num;

    virtual std::string to_json() const;
    virtual void from_json(const std::string& str);
  };
}

bool operator==(const std::string& str, const messages::MESSAGE m);
bool operator!=(const std::string& str, const messages::MESSAGE m);

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
