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

      border.width: 2
      border.color: Qt.darker(parent.color)
    }
  }
}
