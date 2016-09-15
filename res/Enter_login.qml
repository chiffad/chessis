import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

Item
{
  id: _root

  property alias _error : error_text.visible
  signal login_entered(string login)

  height: parent.height/5
  width: parent.width/2

  Rectangle
  {
    id: root_rect

    height: parent.height
    width: parent.width

    color: "light grey"

    border.color: "black"
    border.width: 2
    radius: 5

    Rectangle
    {
      id: input_rect

      anchors.centerIn: parent
      height: parent.height / 5
      width: parent.width - (parent.width / 10)

      border.color: "black"
      border.width: 1
      radius: 3
      color: "white"

      TextInput
      {
        id: _text_input

        visible: parent.visible
        focus: visible
        wrapMode: TextInput.Wrap
        color: "black"

        font.pixelSize: parent.height - parent.height/3

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 2
        anchors.rightMargin: 2
        anchors.bottomMargin: 2
        maximumLength: 20

        clip: true

        onAccepted:
        {
          _root.login_entered(_text_input.text)
          _text_input.clear()
        }
      }
    }

    Text
    {
      id: error_text

      anchors.bottom: parent.bottom
      anchors.bottomMargin: 5
      anchors.horizontalCenter: parent.horizontalCenter

      text: "Wrong login!"

      font.pointSize: 9
      color: "red"
      visible: false
      opacity:0.7
    }
  }

  DropShadow
  {
    anchors.fill: root_rect
    horizontalOffset: 7
    verticalOffset: 7
    radius: root_rect.radius
    samples: 15
    spread: 0.3
    color: "#80000000"
    source: root_rect
  }
}
