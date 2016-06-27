import QtQuick 2.0

Rectangle
{
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: rADIUS

  Item
  {
    id: _text
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: bORDER_WIDTH
    anchors.verticalCenter: parent.verticalCenter
    width: parent.width - cELL_SIZE - parent.width/10

    Text
    {
      anchors.left: parent.left
      anchors.verticalCenter: parent.verticalCenter
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

    width: cELL_SIZE
    height: width

    source: FigureModel.get_move_turn_color
  }
}
