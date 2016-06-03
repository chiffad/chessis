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

  MoveOutput
  {
    id: _moveOutput
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: bORDER_WIDTH

    width: parent.width
    height: parent.height/4
  }

  MoveTurn
  {
    id: _moveTurn
    anchors.left: parent.left
    anchors.top: _moveOutput.bottom
    anchors.topMargin: bORDER_WIDTH
    width: parent.width
    height: parent.height/8
  }

  Button
  {
    id: _back_move_button
    anchors.left: parent.left
    anchors.top: _moveTurn.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Back move"

    onClicked:
    {
      FigureModel.back_move(true);
    }
  }

  Button
  {
    id: _start_new_game
    anchors.left: _back_move_button.right
    anchors.right: parent.right
    anchors.top: _moveTurn.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "New game"

    onClicked:
    {
      FigureModel.start_new_game();
    }
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
      _file_dialog.selectExisting = true
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
      _file_dialog.selectExisting = false
    }
  }

  FileDialog
  {
    id: _file_dialog

    property bool _is_moves_from_file: false

    visible: false
    title: "Please choose a file"
    folder: shortcuts.home

    onAccepted:
    {
      parent.visible = false;
      FigureModel.path_to_file(_file_dialog.fileUrls, _file_dialog._is_moves_from_file);
    }
  }

  Rectangle
  {
    id: _connection_status
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    height: 30
    color: parent.color

    Text
    {
      anchors.fill: parent
      text: FigureModel.udp_connection_status
    }
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
  }
}
