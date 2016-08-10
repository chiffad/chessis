import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  property alias model: _grid.model

  readonly property int eLEMENT_HEIGHT: 20

  anchors.topMargin: border.width

  border.color: "black"
  radius: rADIUS

  GridView
  {
    id: _grid

    anchors.fill: parent
    anchors.margins: parent.border.width
    clip: true

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
          parent.color = "lightskyblue"
          FigureModel.run_command(hISTORY, index)
        }
        onReleased: parent.color = (index % 2) == 1 ? "navajowhite" : "lightyellow"
      }
    }
  }
}
