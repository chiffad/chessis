import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  readonly property int eLEMENT_HEIGHT: 20

  border.width: bORDER_WIDTH
  border.color: "black"
  radius: rADIUS

  GridView
  {
    anchors.fill: parent
    anchors.margins: bORDER_WIDTH
    clip: true

    model: FigureModel.get_moves_history

    cellWidth: parent.width / 2 - 2
    cellHeight: eLEMENT_HEIGHT

    delegate:  Rectangle
    {
      id: _moveNumber
      width: parent.width / 2
      height: eLEMENT_HEIGHT
      radius: rADIUS

      color: (index % 2) == 1 ? "navajowhite" : "lightyellow"

      border.width: 1
      border.color: Qt.darker(color)

      Text
      {
        text: (index % 2 == 0 ? ((index / 2) + 1) + ". " : "") + modelData;
        anchors.horizontalCenter: parent.horizontalCenter
      }

      MouseArea
      {
        anchors.fill: parent
        onPressed:
        {
          parent.color = "	lightskyblue"
          FigureModel.run_command(hISTORY, index)
        }
        onReleased: parent.color = (index % 2) == 1 ? "navajowhite" : "lightyellow"
      }    
    }
  }
}
