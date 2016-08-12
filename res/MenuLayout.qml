import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Rectangle
{
  id: _root

  property alias _move_output_model: _move_output.model
  property alias _move_turn_im_source: _move_turn.im_source
  property alias _connection_status_text: _connection_status.text
  property alias _command_field_current_ind: _command_field.currentIndex
  property alias _command_field_model: _command_field.model

  signal run_command(string command, int num)
  signal work_with_file(string path_to_file, bool is_from_file)

  color: "bisque"

  readonly property int bORDER_WIDTH: 2
  readonly property int rADIUS: 5

  border.width: bORDER_WIDTH
  border.color: "black"

  readonly property string sHOW_OPPONENT : "show opponent"
  readonly property string hELP_WORD : "help"
  readonly property string mOVE_WORD : "move"
  readonly property string bACK_MOVE : "back"
  readonly property string nEW_GAME : "new game"
  readonly property string sHOW_ME : "show me"
  readonly property string hISTORY : "to history"

  MoveOutput
  {
    id: _move_output

    anchors.left: parent.left
    anchors.top: parent.top

    width: parent.width
    height: parent.height/4
    border.width: bORDER_WIDTH
    radius: rADIUS

    delegate:  Rectangle
    {
      id: _moveNumber

      width: parent.width / 2
      height: eLEMENT_HEIGHT
      radius: parent.radius

      color: (index % 2) == 1 ? "navajowhite" : "lightyellow"

      border.width: 1
      border.color: Qt.darker(color)

      Text
      {
        text: (index % 2 == 0 ? ((index / 2) + 1) + ". " : "") + modelData;
        anchors.horizontalCenter: parent.horizontalCenter
      }

      MouseArea
      {
        anchors.fill: parent
        onPressed:
        {
          parent.color = "lightskyblue"
          _root.run_command(hISTORY, index)
        }
        onReleased: parent.color = (index % 2) == 1 ? "navajowhite" : "lightyellow"
      }
    }
  }

  MoveTurn
  {
    id: _move_turn

    anchors.left: parent.left
    anchors.top: _move_output.bottom
    width: parent.width
    height: parent.height/8

    border.width: bORDER_WIDTH
  }

  Button
  {
    id: _back_move_button

    anchors.left: parent.left
    anchors.top: _move_turn.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Back move"

    onClicked: _root.run_command(bACK_MOVE,0);
  }

  Button
  {
    id: _start_new_game

    anchors.left: _back_move_button.right
    anchors.right: parent.right
    anchors.top: _move_turn.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "New game"

    onClicked: _root.run_command(nEW_GAME,0)
  }

  Button
  {
    id: _moves_from_file

    anchors.left: parent.left
    anchors.top: _start_new_game.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Load file"

    onClicked:
    {
      _file_dialog.visible = true;
      _file_dialog._is_moves_from_file = true;
    }
  }

  Button
  {
    id: _moves_to_file

    anchors.left:  _moves_from_file.right
    anchors.right: parent.right
    anchors.top: _start_new_game.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Create file"

    onClicked:
    {
      _file_dialog.visible = true;
      _file_dialog._is_moves_from_file = false;
    }
  }

  FileDialog
  {
    id: _file_dialog

    property bool _is_moves_from_file: false

    visible: false
    folder: shortcuts.home
    selectMultiple: false
    selectExisting: !_is_moves_from_file

    onAccepted:
    {
      parent.visible = false;
      _root.work_with_file(_file_dialog.fileUrls, _file_dialog._is_moves_from_file)
    }
  }

  ConnectionStatus
  {
    id: _connection_status

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.leftMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    anchors.bottomMargin: bORDER_WIDTH
  }

  CommandField
  {
    id: _command_field

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: _moves_from_file.bottom
    anchors.bottom: _connection_status.top
    anchors.topMargin: bORDER_WIDTH
    anchors.bottomMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH
    radius: rADIUS

    onText_inp_accepted:
    {
      _root.run_command(text_inp.text,0)
      text_inp.clear()
    }
  }
}
