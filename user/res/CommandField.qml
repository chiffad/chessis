import QtQuick 2.12
import QtQuick.Controls 1.4

Rectangle {
  id: root

  property alias currentIndex: list_view.currentIndex
  property alias model: list_view.model
  property alias text_inp: text_input
  signal text_inp_accepted(string text)

  border.color: "white"
  color: "blue"
  visible: false

  ListView {
    id: list_view

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.top: parent.top
    anchors.bottom: text_input.top

    clip:true

    delegate:
      Text {
        width: root.width
        color: "yellow"
        wrapMode: Text.Wrap
        text: modelData
      }
  }

  TextInput {
    id: text_input

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.bottomMargin: 2

    visible: parent.visible
    clip: true
    wrapMode: TextInput.Wrap
    color: "white"

    onAccepted: {
      root.text_inp_accepted(text_input.text);
      text_input.text = "";
    }
  }

  Keys.onPressed: {
    if(event.key === Qt.Key_F1) root.visible = true;
    if(event.key === Qt.Key_Escape) root.visible = false;
  }
}
