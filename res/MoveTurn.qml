import QtQuick 2.0

Rectangle
{
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 5

  Item
  {
    id: _text
    anchors.left: parent.left
    width: parent.width - cELL_SIZE

    Text
    {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      font.family: "Helvetica"
      font.pointSize: 14
      color: "black"

      text: "Move turn: "
    }
  }

  Image
  {
    anchors.left: _text.right
    anchors.verticalCenter: parent.verticalCenter

    source: Integration.move_turn_color
  }
}

