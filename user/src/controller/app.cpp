#include "controller/app.hpp"
#include "client/messages.h"

#include <QQmlContext>

#include <spdlog/spdlog.h>

namespace controller {

app_t::app_t(const board_graphic_t::command_requested_callback_t& callback)
  : menu_layout_{callback}
  , board_{callback}
  , login_input_{[callback](const std::string& login, const std::string& pwd) { callback(msg::prepare_for_send(msg::login_t(login, pwd))); }}
  , message_processor_{menu_layout_, board_, login_input_}
  , engine_{}
{
  engine_.rootContext()->setContextProperty("MenuLayoutController", &menu_layout_);
  engine_.rootContext()->setContextProperty("FigureModel", &board_);
  engine_.rootContext()->setContextProperty("LoginInputController", &login_input_);
  engine_.load(QUrl(QStringLiteral("qrc:/res/app.qml")));
}

app_t::~app_t() = default;

void app_t::process(const std::string& server_message)
{
  message_processor_.process_server_message(server_message);
}

} // namespace controller