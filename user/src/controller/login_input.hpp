#pragma once

#include <QObject>
#include <QString>

namespace controller {

class login_input_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString login READ login WRITE set_login NOTIFY login_changed)
  Q_PROPERTY(QString pwd READ pwd WRITE set_pwd NOTIFY pwd_changed)
  Q_PROPERTY(bool login_valid READ login_valid NOTIFY login_valid_changed)
  Q_PROPERTY(bool pwd_valid READ pwd_valid NOTIFY pwd_valid_changed)
  Q_PROPERTY(QString error_message READ error_message NOTIFY error_message_changed)

public:
  Q_INVOKABLE bool set_login(const QString& login, const QString& pwd);

public:
  using login_entered_callback_t = std::function<void(const std::string&, const std::string& pwd)>;

public:
  explicit login_input_t(const login_entered_callback_t& callback);
  ~login_input_t() override;

  QString login() const;
  void set_login(const QString& l);

  QString pwd() const;
  void set_pwd(const QString& pwd);

  bool login_valid() const;
  void set_login_valid(bool valid);

  bool pwd_valid() const;
  void set_pwd_valid(bool valid);

  QString error_message() const;
  void set_error_message(const QString& str);

  void get_login(const QString& error_mess = QString()) { emit enter_login(error_mess); }

signals:
  void login_changed();
  void pwd_changed();
  void login_valid_changed();
  void pwd_valid_changed();
  void error_message_changed();
  void enter_login(const QString& error_mess);

private:
  QString login_;
  QString pwd_;
  bool login_valid_;
  bool pwd_valid_;
  QString error_message_;

  login_entered_callback_t login_entered_callback_;
};

} // namespace controller