import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import Constants 1.0

Window {
  id: _root

  visible: true
  width: Constants.board_layout_size + Constants.menu_size
  height: Constants.board_layout_size

  maximumHeight: height
  minimumHeight: height
  maximumWidth: width
  minimumWidth: width

  MenuLayout {
    id: _menu_layout

    height: parent.height
    anchors.left: board.right
    anchors.right: parent.right

    _move_output_model: FigureModel.get_moves_history
    _move_turn_im_source: FigureModel.get_move_turn_color
    _command_field_model: FigureModel.get_commands_hist
    _command_field_current_ind: FigureModel.get_last_command_hist_ind
    _connection_status_text: FigureModel.get_udp_connection_status

    onRun_command: FigureModel.run_command(command, num)
    onWork_with_file: FigureModel.path_to_file(path_to_file, is_from_file);
  }

  BoardLayout {
    id: board    
  }

  Login_input {
    id: _login_input

    visible: false
    anchors.centerIn: parent
    z: _root.Constants.enter_login_z

    onData_entered: {
      if(login.length && pwd.length && FigureModel.set_login(login, pwd)) {
        _error_visible = false
        _login_input.visible = false
        _menu_layout._text_inp.focus = true
      }
      else {
        _error_visible = true
        _error_text = "Wrong login/pwd!"
      }
    }
  }

  Connections {
    target: FigureModel

    function onEnter_login(error_text) {
      _login_input.visible = true
      _login_input._error_visible = true
      _login_input._error_text = error_text
    }
  }
}
