import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  readonly property int eLEMENT_HEIGHT: 20

  color: "darkkhaki"
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 5

  ListView
  {
    width: parent.width
    height: parent.height - eLEMENT_HEIGHT

    model: FigureModel.moves_history

    delegate: Rectangle
    {
      id: _moveNumber
      anchors.left: parent.left
      width: parent.width
      height: eLEMENT_HEIGHT
      color: "lightyellow"//(currentIndex % 2) != 0 ? "lightyellow" : "gold"

      border.width: 2
      border.color: "black"//Qt.darker(color)

      Text { text: modelData }

     /* MouseArea
      {
        anchors.fill: parent
        onClicked: FigureModel.move_to_history_index(currentIndex)
      }*/
    }
  }
}
