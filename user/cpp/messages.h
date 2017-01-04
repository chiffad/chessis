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
  struct login_t//+ server get
  {
    std::string login;
//    std::string pwd;

    std::string to_json() const;
    void from_json(const std::string& str);
  };

/*  struct incorrect_log_t //+ server get
  {
  };
*/
  struct move_t //+ server get
  {
    std::string data;

    std::string to_json() const;
    void from_json(const std::string& str);
  };

/*  struct back_move_t //+ server get
  {
  };
*/
  struct go_to_history_t//+ server get
  {
    int index;

    std::string to_json() const;
    void from_json(const std::string& str);
  };
/*
  struct new_game_t//+ server get
  {
  };
*/
  struct inf_request_t// + client get
  {
    std::string data;

    std::string to_json() const;
    void from_json(const std::string& str);
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
  struct game_inf_t// + client get
  {
    std::string board_mask;
    std::string moves_history;
    bool is_mate;
    int move_num;

    std::string to_json() const;
    void from_json(const std::string& str);
  };

  struct my_except : public std::exception
  {
    my_except(const std::string& err) : std::exception(), error(err)
    {}
    virtual const char* what() const throw()
      { return error.data(); }

    private:
      std::string error;
  };
}

bool operator==(const std::string& str, const messages::MESSAGE m);
bool operator!=(const std::string& str, const messages::MESSAGE m);

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
