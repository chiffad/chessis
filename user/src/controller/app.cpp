#include "controller/app.hpp"
#include <messages/messages.hpp>

#include <QQmlContext>

#include <spdlog/spdlog.h>

namespace controller {

app_t::app_t(const command_requested_callbacks_t& callbacks)
  : menu_layout_{callbacks}
  , board_{[callbacks, this](const std::string& move_msg) {
    menu_layout_.add_to_command_history("Command: move " + QString::fromStdString(move_msg));
    callbacks.move(move_msg);
  }}
  , login_input_{callbacks.login}
  , message_processor_{menu_layout_, board_, login_input_}
  , engine_{}
{
  engine_.rootContext()->setContextProperty("MenuLayoutController", &menu_layout_);
  engine_.rootContext()->setContextProperty("BoardController", &board_);
  engine_.rootContext()->setContextProperty("LoginInputController", &login_input_);

  board_.set_context_properties(engine_);

  engine_.load(QUrl(QStringLiteral("qrc:/res/app.qml")));
}

app_t::~app_t() = default;

void app_t::server_status_changed(const bool server_online)
{
  message_processor_.server_status_changed(server_online);
}

} // namespace controller
