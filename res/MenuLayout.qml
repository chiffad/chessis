import QtQuick 2.0
import QtQuick.Controls 1.4

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
    id: _moves_to_file
    anchors.right: parent.right
    anchors.top: _start_new_game.top
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Create file"

    onClicked:
    {
      _path_to_file.visible = true;
      _path_to_file._is_moves_from_file = false;
    }
  }

  Button
  {
    id: _moves_from_file
    anchors.right: parent.right
    anchors.top: _start_new_game.top
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    width: parent.width/2
    height: parent.height/10

    text: "Load file"

    onClicked:
    {
      _path_to_file.visible = true;
      _path_to_file._is_moves_from_file = true;
    }
  }

  Rectangle
  {
    id: _path_to_file
    anchors.horizontalCenter: paretn
    anchors.bottom: parent.bottom
    height: 30
    visible: false
    border.width: bORDER_WIDTH
    border.color: "black"

    property bool _is_moves_from_file: false

    TextInput
    {
      anchors.fill: parent
      visible: parent
      text: "Enter full path & file name"

      onAccepted:
      {
        parent.visible = false;
        FigureModel.path_to_file(text, _path_to_file._is_moves_from_file);
      }
    }
  }
}
