import QtQuick 2.0

Rectangle
{
  color: "darkkhaki"
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 5

  signal newMove

  Rectangle
  {
    id: _moveNumber
    anchors.left: parent.left
    width: 10

    border.width: 2
    border.color: Qt.darker(parent.color)

  }
  ListModel
  {


  }
}

