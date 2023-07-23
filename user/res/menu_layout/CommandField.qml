import QtQuick 2.12
import QtQuick.Controls 1.4

import Constants 1.0

Item {
  id: root

  property alias currentIndex: list_view.currentIndex
  property alias model: list_view.model
  property alias text_inp: text_input
  signal text_inp_accepted(string text)

  Text {
    width: root.width
    color: "black"
    wrapMode: Text.Wrap
    text: "Press F1 to open Commands Field"
  }

  Rectangle {
    id: command_field
    anchors.fill: parent
    border.color: "black"
    //color: "write"
    radius: Constants.radius
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

      delegate: Text {
        width: root.width
        color: "black"
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

      clip: true
      wrapMode: TextInput.Wrap
      color: "white"

      onAccepted: {
        root.text_inp_accepted(text_input.text);
        text_input.text = "";
      }
    }
  }

  Keys.onPressed: {
    if(event.key === Qt.Key_F1) command_field.visible = true;
    if(event.key === Qt.Key_Escape) command_field.visible = false;
  }
}