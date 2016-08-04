import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root
  border.color: "white"
  radius: rADIUS
  color: "blue"
  visible: false

  ListView
  {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: 2
    anchors.rightMargin: 2
    anchors.top: parent.top
    anchors.bottom: text_input.top

    clip:true

    model: FigureModel.get_commands_hist
    delegate:
      Text
      {
        width: _root.width
        color: "yellow"
        wrapMode: Text.Wrap
        text: modelData
      }

    currentIndex: FigureModel.get_last_elem_ind
  }

  TextInput
  {
    id: text_input
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
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
      text = "";
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
