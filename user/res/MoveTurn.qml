import QtQuick 2.12

Rectangle {
  id: root
  property alias im_source: move_turn_image.source

  anchors.topMargin: border.width
  border.color: "black"

  Text {
    id: text

    anchors.left: parent.left
    anchors.leftMargin: root.border.width
    anchors.verticalCenter: parent.verticalCenter
    font.family: "Helvetica"
    font.pointSize: 14
    color: "black"

    text: "Move turn: "
  }

  Image {
    id: move_turn_image
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: text.right

    width: height
    height: root.height
  }
}
