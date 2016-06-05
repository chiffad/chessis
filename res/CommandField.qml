import QtQuick 2.5
import QtQuick.Controls 1.4

Rectangle
{
  id: _root
  width: parent.width
  border.width: bORDER_WIDTH
  border.color: "white"
  radius: rADIUS
  color: "darkorchid"
  visible: false

  ListView
  {
    id: _history_view
    model: FigureModel.commands_list
    delegate:
      Text
      {
        width: _root.width
        color: "grey"
        wrapMode: Text.Wrap
        text: modelData
      }
  }

  TextInput
  {
    anchors.top: _history_view.bottom
    width: parent.width
    visible: parent.visible
    clip: true
    focus: true
    wrapMode: TextInput.Wrap


    onAccepted:
      FigureModel.run_command(text);
  }

  Keys.onPressed:
  {
    if(event.key == Qt.Key_AsciiTilde)
      _root.visible = true;
    if(event.key == Qt.Key_Escape)
      _root.visible = false
  }
}
