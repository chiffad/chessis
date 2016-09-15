import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

Rectangle
{
  id: _root

  property alias _error : error_text.visible

  signal login_entered(string login)

  height: parent.height/5
  width: parent.width/2

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

  DropShadow
  {
    anchors.fill: input_rect
    horizontalOffset: 7
    verticalOffset: 7
    radius: input_rect.radius
    samples: 17
    spread: 0.5
    color: "#80000000"
    source: input_rect
  }

  Text
  {
    id: error_text

    anhors.top: input_rect.bottom
    anhors.bottom: parent.bottom
    anhors.left: parent.left
    anhors.right: parent.right

    text: "Wrong login!"

    font.pointSize: (parent.height - input_rect.height) / 2
    color: "red"
    visible: false
  }
}
