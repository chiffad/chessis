import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import Constants 1.0

Window {
  id: root

  visible: true
  width: Constants.board_layout_size + Constants.menu_size
  height: Constants.board_layout_size

  maximumHeight: height
  minimumHeight: height
  maximumWidth: width
  minimumWidth: width

  MenuLayout {
    id: menu_layout

    height: parent.height
    anchors.left: board.right
    anchors.right: parent.right

    move_output_model: FigureModel.get_moves_history
    move_turn_img_source: FigureModel.get_move_turn_color
    command_field_model: FigureModel.get_commands_hist
    command_field_current_ind: FigureModel.get_last_command_hist_ind
    connection_status_text: FigureModel.get_udp_connection_status

    onRun_command: FigureModel.run_command(command, num)
    onWork_with_file: FigureModel.path_to_file(path_to_file, is_from_file);
  }

  BoardLayout {
    id: board    
  }

  LoginInput {
    id: login_input

    visible: false
    anchors.centerIn: parent
    z: root.Constants.enter_login_z

    onData_entered: {
      if(login.length && pwd.length && FigureModel.set_login(login, pwd)) {
        error_visible = false
        login_input.visible = false
        menu_layout._text_inp.focus = true
      }
      else {
       _error_visible = true
        error_text = "Wrong login/pwd!"
      }
    }
  }

  Connections {
    target: FigureModel

    function onEnter_login(error_text) {
      login_input.visible = true
      login_input.error_visible = true
      login_input.error_text = error_text
    }
  }
}
