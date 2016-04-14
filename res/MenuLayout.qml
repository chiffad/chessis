import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle
{
  color: "bisque"

  readonly property int bORDER_WIDTH: 2
  readonly property int rADIUS: 5

  border.width: bORDER_WIDTH
  border.color: "black"

  /*Button //work
  {
    id: _back_move_button
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH
    radius: rADIUS
    width: parent.width/2
    height: parent.height/10

    text: "Back move"

    onClicked:
    {
      FigureModel.back_move(true);
    }
  }*/

  /*Button //work
  {
    id: _start_new_game
    anchors.right: parent.right
    anchors.top: _back_move_button.bottom
    anchors.topMargin: bORDER_WIDTH
    anchors.rightMargin: bORDER_WIDTH
    radius: rADIUS
    width: parent.width/2
    height: parent.height/10

    text: "New game"

    onClicked:
    {
      FigureModel.start_new_game();
    }
  }*/

  /*MoveTurn // work!
  {
    id: _moveTurn
    anchors.left: parent.left
    anchors.top: _start_new_game.bottom
    anchors.topMargin: bORDER_WIDTH
    width: parent.width
    height: parent.height/8
  }*/

  MoveOutput // fix need
  {
    anchors.left: parent.left
    //anchors.top: _moveTurn.bottom
    anchors.topMargin: bORDER_WIDTH

    width: parent.width
    height: parent.height/4
  }
}
