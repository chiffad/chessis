import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root

  property alias currentIndex: _list_view.currentIndex

  property alias model: _list_view.model

  border.color: "white"
  radius: rADIUS
  color: "blue"
  visible: false

  ListView
  {
    id: _list_view

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.top: parent.top
    anchors.bottom: text_input.top

    clip:true

    delegate:
      Text
      {
        width: _root.width
        color: "yellow"
        wrapMode: Text.Wrap
        text: modelData
      }
  }

  TextInput
  {
    id: text_input

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.bottomMargin: 2
    visible: parent.visible
    clip: true
    focus: true
    wrapMode: TextInput.Wrap
    color: "white"

    onAccepted:
    {
      FigureModel.run_command(text);
      clear();
    }
  }

  Keys.onPressed:
  {
    if(event.key === Qt.Key_F1)
    {
      text_input.text = ""
      _root.visible = true;
    }
    if(event.key === Qt.Key_Escape)
      _root.visible = false
  }
}
