#pragma once

#include <QObject>
#include <QString>

namespace controller {

class login_input_t : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString error_message READ error_message NOTIFY error_message_changed)

public:
  Q_INVOKABLE bool set_login(const QString& login, const QString& pwd);

public:
  using login_entered_callback_t = std::function<void(const std::string&, const std::string& pwd)>;

public:
  explicit login_input_t(const login_entered_callback_t& callback);
  ~login_input_t() override;

  QString error_message() const;
  void set_error_message(const QString& str);

  void get_login(const QString& error_mess = QString());

signals:
  void error_message_changed();
  void enter_login();

private:
  QString error_message_;

  login_entered_callback_t login_entered_callback_;
};

} // namespace controller