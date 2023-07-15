import QtQuick 2.12

FocusScope {
  id: root

  property alias text: text_input.text
  signal accepted()

  Rectangle {
    anchors.fill: parent
    border.color: "black"
    border.width: 1
    radius: 3
    color: text_input.activeFocus ? "orange" : "white"

    TextInput {
      id: text_input

      focus: true
      wrapMode: TextInput.Wrap
      color: "black"

      font.pixelSize: root.height - root.height/3

      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.leftMargin: 2
      anchors.rightMargin: 2
      anchors.bottomMargin: 2
      maximumLength: 20

      clip: true

      onAccepted: root.accepted()
    }
  }
}    
