#ifndef __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
#define __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD

#include <string>
#include <memory>

namespace messages
{
  enum MESSAGE {HELLO_SERVER = 1, MESSAGE_RECEIVED, IS_SERVER_LOST, IS_CLIENT_LOST,
                OPPONENT_INF, MY_INF, GET_LOGIN, LOGIN, INCORRECT_LOG,
                MOVE, BACK_MOVE, GO_TO_HISTORY, NEW_GAME, FROM_FILE, INF_REQUEST,
                SERVER_LOST, SERVER_HERE, CLIENT_LOST, OPPONENT_LOST, GAME_INF};


  struct message_t
  {
    message_t(const int t);
    virtual void gather(const std::string& str);//cut str and innitialize all values with resulting pieces
    int type;
  };

  struct go_to_history_t : public message_t
  {
    go_to_history_t();
    virtual void gather(const std::string& str);
    int hist_ind;
  };

  struct inf_request_t : public message_t
  {
    inf_request_t();
    virtual void gather(const std::string& str);
    std::string data;
  };

  struct helper
  {
    static std::shared_ptr<message_t> get_and_init_message_struct(const std::string& str);
  };

}

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
