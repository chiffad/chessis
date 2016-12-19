#ifndef __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
#define __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD

namespace messages
{
  enum MESSAGE {HELLO_SERVER = 1, MESSAGE_RECEIVED, IS_SERVER_LOST, IS_CLIENT_LOST,
                OPPONENT_INF, MY_INF, GET_LOGIN, LOGIN, INCORRECT_LOG,
                MOVE, BACK_MOVE, GO_TO_HISTORY, NEW_GAME, FROM_FILE, INF_REQUEST,
                SERVER_LOST, SERVER_HERE, CLIENT_LOST, OPPONENT_LOST, GAME_INF};
/*

  template<typename T>
  class foo_t
  {
  public:
    foo_t(const T& _1, const int ser_num);
    const T& get() const;
    const int get_ser_num() const;
    void initialize(const std::string s);

  private:
    T t;
    int ser_num;
  };

  struct some_message_strut_1
  {
    void gather(string str)//cut str and innitialize all values with resulting pieces
    string something;
    string something1;
    ....
  };

  struct some_message_strut_2
  {
    void gather(string str) //cut str and innitialize all values with resulting pieces
    string something;
    string something1;
    ...
  };
*/
}

#endif // __MY_ENUM_H__UILGBAWLIDBAWYGDTAGFDWTYAD
