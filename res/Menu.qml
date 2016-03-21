import QtQuick 2.0

Rectangle
{
  color: "bisque"

  readonly  property int bORDER_WIDTH: 3

  border.width: bORDER_WIDTH
  border.color: "black"

  MoveTurn
  {
    anchors.left: parent.left
    anchors.top: parent.top
    width: parent.width
    height: parent.height/8
  }

  MoveOutput
  {
    anchors.left: parent.left
    anchors.top: _moveTurn.bottom
    anchors.topMargin: 5

    width: parent.width
    height: parent.height/4
  }
}

