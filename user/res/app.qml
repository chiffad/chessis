import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import Constants 1.0

import "login"
import "menu_layout"
import "board"

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

    move_output_model: MenuLayoutController.moves_history
    move_turn_img_source: MenuLayoutController.white_move_turn ? "qrc:/res/img/w_k.png" : "qrc:/res/img/b_K.png"
    command_field_model: MenuLayoutController.commands_hist
    command_field_current_ind: MenuLayoutController.last_command_hist_ind
    connection_status_text: MenuLayoutController.connection_status

    onWork_with_file: MenuLayoutController.path_to_file(path_to_file, is_from_file);
  }

  LoginInput {
    id: login_input

    anchors.centerIn: parent
    height: root.height/3
    width: root.width/2    
  }
}
