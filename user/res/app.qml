import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import Constants 1.0

import "login"
import "menu_layout"

Window {
  id: root

  visible: true
  width: Constants.board_layout_size + Constants.menu_size
  height: Constants.board_layout_size

  maximumHeight: height
  minimumHeight: height
  maximumWidth: width
  minimumWidth: width

  BoardLayout {
    id: board    
    anchors.left: parent.left
  }

  MenuLayout {
    id: menu_layout

    height: parent.height
    anchors.left: board.right
    anchors.right: parent.right
    focus: !login_input.visible 

    move_output_model: FigureModel.get_moves_history
    move_turn_img_source: FigureModel.get_move_turn_color
    command_field_model: FigureModel.get_commands_hist
    command_field_current_ind: FigureModel.get_last_command_hist_ind
    connection_status_text: FigureModel.get_udp_connection_status

    onRun_command: FigureModel.run_command(command, num)
    onWork_with_file: FigureModel.path_to_file(path_to_file, is_from_file);
  }

  LoginInput {
    id: login_input

    anchors.centerIn: parent
    height: root.height/3
    width: root.width/2    
  }
}
