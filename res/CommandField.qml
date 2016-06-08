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
    width: parent.width
    height: count * 20
    model: FigureModel.commands_list
    delegate:
      Text
      {
        width: _root.width
        height: 20
        color: "grey"
        wrapMode: Text.Wrap
        text: modelData
      }
  }

  TextInput
  {
    anchors.top: _history_view.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: _root.border.width
    anchors.rightMargin: _root.border.width
    visible: parent.visible
    clip: true
    focus: true
    wrapMode: TextInput.Wrap

    onAccepted:
    {
      FigureModel.run_command(text);
      text = "";
    }
  }

  Keys.onPressed:
  {
    if(event.key == Qt.Key_AsciiTilde)
      _root.visible = true;
    if(event.key == Qt.Key_Escape)
      _root.visible = false
  }
}
