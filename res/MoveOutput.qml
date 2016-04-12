import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  color: "darkkhaki"
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 10

  ListView
  {
    width: parent.width
    height: parent.height

    model: FigureModel.moves_history

    delegate: Rectangle
    {
      id: _moveNumber
      anchors.left: parent.left
      width: 10
      color: (count % 2) != 0 ? 'lightyellow' : 'gold'

      border.width: 1
      border.color: Qt.darker(parent.color)

      Text { text: modelData }

      MouseArea
      {
        anchors.fill: parent
        onClicked: FigureModel.move_to_history_index(currentIndex)
      }
    }
  }
}
