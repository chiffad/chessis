import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

Rectangle
{
  color: "bisque"

  readonly property int bORDER_WIDTH: 2
  readonly property int rADIUS: 5

  border.width: bORDER_WIDTH
  border.color: "black"

  readonly property string hELP_WORD : "help"
  readonly property string mOVE_WORD : "move"
  readonly property string bACK_MOVE : "back"
  readonly property string sHOW_ME : "show me"
  readonly property string nEW_GAME : "new game"
  readonly property string hISTORY : "to history"
  readonly property string sHOW_OPPONENT : "show opponent"

  MoveOutput
  {
    id: _moveOutput

    anchors.left: parent.left
    anchors.top: parent.top

    width: parent.width
    height: parent.height/4
    border.width: bORDER_WIDTH
    radius: rADIUS

    model: FigureModel.get_moves_history

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
          FigureModel.run_command(hISTORY, index)
        }
        onReleased: parent.color = (index % 2) == 1 ? "navajowhite" : "lightyellow"
      }
    }
  }

  MoveTurn
  {
    id: _move_turn

    anchors.left: parent.left
    anchors.top: _moveOutput.bottom
    width: parent.width
    height: parent.height/8

    border.width: bORDER_WIDTH

    im_source: FigureModel.get_move_turn_color
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

    onClicked:
      FigureModel.run_command(bACK_MOVE);
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

    onClicked:
      FigureModel.run_command(nEW_GAME);
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
      FigureModel.path_to_file(_file_dialog.fileUrls, _file_dialog._is_moves_from_file);
    }
  }

  Rectangle
  {
    id: _connection_status

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.leftMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    anchors.bottomMargin: bORDER_WIDTH
    height: 20
    color: parent.color

    Text
    {
      id: _text
      anchors.fill: parent
      text: FigureModel.get_udp_connection_status
    }
  }

  CommandField
  {
    id: _command_field

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: _moves_from_file.bottom
    anchors.bottom: _connection_status.top
    anchors.topMargin: bORDER_WIDTH
    anchors.bottomMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH

    currentIndex: FigureModel.get_last_elem_ind
    model: FigureModel.get_commands_hist

    onText_inp_accepted:
    {
      FigureModel.run_command(text_inp.text);
      text_inp.clear()
    }
  }
}
