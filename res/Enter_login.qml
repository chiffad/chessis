import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root

  property alias login_inp: _text_input
  signal login_entered(string login)

  height: parent.height/5
  width: parent.width/5

  color: "light grey"

  border.color: "black"
  border.width: 5
  radius: 10

  Rectangle
  {
    anchors.centerIn: parent
    height: parent.height / 5
    width: parent.width - (parent.width / 10)

    border.color: "black"
    border.width: 2
    radius: 10
    color: "white"

    TextInput
    {
      id: _text_input

      anchors.centerIn: parent
      clip: true
      focus: _root.visible
      wrapMode: TextInput.Wrap
      color: "white"

      onAccepted:
      {
        _root.login_entered(_text_input.text)
        _text_input.clear()
      }
    }
  }
}