import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  readonly property int eLEMENT_HEIGHT: 20

  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 5

  GridView
  {
    width: parent.width
    height: parent.height - eLEMENT_HEIGHT

    cellWidth: parent.width / 2
    cellHeight: eLEMENT_HEIGHT
    clip: true

    model: FigureModel.moves_history
    delegate: Rectangle
    {
      id: _moveNumber
      anchors.left: parent.left
      anchors.leftMargin: 1
      anchors.rightMargin: 1
      radius: 5

      color: (index % 2) == 1 ? "navajowhite" : "lightyellow"

      border.width: 1
      border.color: Qt.darker(color)

      Text { text: modelData }

     /* MouseArea
      {
        anchors.fill: parent
        onClicked: FigureModel.go_to_history_index(index)
      }*/
    }
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
    focus: true
  }
}
