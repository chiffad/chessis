import QtQuick 2.5

Rectangle
{
  property alias im_source: _image.source

  anchors.topMargin: border.width

  border.color: "black"
  radius: rADIUS

  Item
  {
    id: _text
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.leftMargin: parent.border.width
    anchors.verticalCenter: parent.verticalCenter
    width: parent.width - cELL_SIZE_X - parent.width/10

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

    width: cELL_SIZE_X
    height: width
  }
}
