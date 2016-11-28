import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root

  property alias currentIndex: _list_view.currentIndex
  property alias model: _list_view.model
  property alias text_inp: _text_input
  signal text_inp_accepted(string text)

  border.color: "white"
  color: "blue"
  visible: false

  ListView
  {
    id: _list_view

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.top: parent.top
    anchors.bottom: _text_input.top

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
    id: _text_input

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

    onAccepted:
    {
      _root.text_inp_accepted(_text_input.text)
      _text_input.clear()
    }
  }

  Keys.onPressed:
  {
    if(event.key === Qt.Key_F1)
    {
      _text_input.clear();
      _root.visible = true;
    }
    if(event.key === Qt.Key_Escape)
      _root.visible = false;
  }
}
