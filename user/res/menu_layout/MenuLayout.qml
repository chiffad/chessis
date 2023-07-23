import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Constants 1.0

FocusScope {
  id: root
  property alias move_output_model: move_output.model
  property alias move_turn_img_source: move_turn.img_source
  property alias connection_status_text: connection_status.text
  property alias command_field_current_ind: command_field.currentIndex
  property alias command_field_model: command_field.model
  property alias text_inp: command_field.text_inp

  signal run_command(string command, int num)
  signal work_with_file(string path_to_file, bool is_from_file)

  Rectangle {
    id: rect

    color: "bisque"
    height: parent.height
    width: parent.width

    readonly property int border_width: 2

    border.width: rect.border_width
    border.color: "black"

    MoveOutput {
      id: move_output

      anchors.left: parent.left
      anchors.top: parent.top

      width: parent.width
      height: parent.height/4
      border.width: rect.border_width
      radius: Constants.radius

      delegate: Rectangle {
        id: move_number

        width: parent.width / 2
        height: move_output.height / 6
        radius: move_output.radius

        color: (index % 2) == 1 ? "navajowhite" : "lightyellow"

        border.width: 1
        border.color: Qt.darker(color)

        Text {
          text: (index % 2 == 0 ? ((index / 2) + 1) + ". " : "") + modelData;
          anchors.horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
          anchors.fill: parent
          onPressed: {
            parent.color = "lightskyblue"
            root.run_command("to history", index)
          }
          onReleased: parent.color = (index % 2) == 1 ? "navajowhite" : "lightyellow"
        }
      }
    }

    MoveTurn {
      id: move_turn

      anchors.left: parent.left     
      anchors.top: move_output.bottom
      width: parent.width
      height: parent.height/8

      radius: Constants.radius
      border.width: rect.border_width
    }

    Button {
      id: back_move_button

      anchors.left: parent.left
      anchors.top: move_turn.bottom
      anchors.topMargin: rect.border_width
      anchors.leftMargin: rect.border_width
      width: parent.width/2
      height: parent.height/10

      text: "Back move"

      onClicked: root.run_command("back",0);
    }

    Button {
      id: start_new_game

      anchors.left: back_move_button.right
      anchors.right: parent.right
      anchors.top: move_turn.bottom
      anchors.topMargin: rect.border_width
      anchors.rightMargin: rect.border_width
      width: parent.width/2
      height: parent.height/10

      text: "New game"

      onClicked: root.run_command("new game", 0)
    }

    Button {
      id: moves_from_file

      anchors.left: parent.left
      anchors.top: start_new_game.bottom
      anchors.topMargin: rect.border_width
      anchors.leftMargin: rect.border_width
      width: parent.width/2
      height: parent.height/10

      text: "Load file"

      onClicked: {
        file_dialog.visible = true;
        file_dialog._is_moves_from_file = true;
      }
    }

    Button {
      id: moves_to_file

      anchors.left:  moves_from_file.right
      anchors.right: parent.right
      anchors.top: start_new_game.bottom
      anchors.topMargin: rect.border_width
      anchors.rightMargin: rect.border_width
      width: parent.width/2
      height: parent.height/10

      text: "Create file"

      onClicked: {
        file_dialog.visible = true;
        file_dialog._is_moves_from_file = false;
      }
    }

    FileDialog {
      id: file_dialog

      property bool _is_moves_from_file: false

      visible: false
      folder: shortcuts.home
      selectMultiple: false
      selectExisting: !_is_moves_from_file

      onAccepted: {
        parent.visible = false;
        root.work_with_file(file_dialog.fileUrls, file_dialog._is_moves_from_file)
      }
    }

    CommandField {
      id: command_field

      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: moves_from_file.bottom
      anchors.bottom: connection_status.top
      anchors.topMargin: rect.border_width
      anchors.bottomMargin: rect.border_width
      anchors.rightMargin: rect.border_width
      anchors.leftMargin: rect.border_width
      focus: true

      onText_inp_accepted: {
        if(text.length)
          root.run_command(text,0)
      }
    }

    ConnectionStatus {
      id: connection_status

      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.leftMargin: rect.border_width
      anchors.rightMargin: rect.border_width
      anchors.bottomMargin: rect.border_width
    }
  }
}