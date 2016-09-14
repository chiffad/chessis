import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root

  signal login_entered(string login)

  height: parent.height/5
  width: parent.width/2

  color: "light grey"

  border.color: "black"
  border.width: 2
  radius: 5

  Rectangle
  {
    anchors.centerIn: parent
    height: parent.height / 5
    width: parent.width - (parent.width / 10)

    border.color: "black"
    border.width: 1
    radius: 2
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
}
