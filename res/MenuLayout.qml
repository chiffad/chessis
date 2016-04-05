import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle
{
  color: "bisque"

  readonly property int bORDER_WIDTH: 2
  readonly property int mARGIN : 3

  border.width: bORDER_WIDTH
  border.color: "black"

  Button
  {
    id: _back_move_button
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH

    width: 100
    height: parent.height/10

    text: "Back move"

    onClicked:
    {
      FigureModel.back_move();
    }
  }

  MoveTurn
  {
    id: _moveTurn
    anchors.left: parent.left
    anchors.top: _back_move_button.bottom
    anchors.topMargin: mARGIN
    width: parent.width
    height: parent.height/8
  }

  MoveOutput
  {
    anchors.left: parent.left
    anchors.top: _moveTurn.bottom
    anchors.topMargin: mARGIN

    width: parent.width
    height: parent.height/4
  }
}
