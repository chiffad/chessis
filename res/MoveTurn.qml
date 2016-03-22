import QtQuick 2.0

Rectangle
{
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 10

  Item
  {
    id: _text
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: mARGIN
    anchors.topMargin: mARGIN
    width: parent.width - cELL_SIZE

    Text
    {
      anchors.left: parent.left
      font.family: "Helvetica"
      font.pointSize: 14
      color: "black"

      text: "Move turn: "
    }
  }

  Image
  {
    id: _image
    anchors.left: _text.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.topMargin: 3
    anchors.rightMargin: 3

    width: cELL_SIZE
    height: width

    source: integration.move_turn_color
  }
}

