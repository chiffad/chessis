import QtQuick 2.0
import QtQml.Models 2.2

Rectangle
{
  color: "darkkhaki"
  border.width: bORDER_WIDTH
  border.color: "black"
  radius: 10

  ListModel{id: _movesHistory}

  ListView
  {
    width: parent.width
    height: parent.height

    model: _movesHistory

    delegate: Rectangle
    {
      id: _moveNumber
      anchors.left: parent.left
      width: 10

      border.width: 2
      border.color: Qt.darker(parent.color)

      Text
      {
        text: model.moveNum
      }

      Text
      {
        text: model.whiteMove
      }

      Text
      {
        text: model.blackMove
      }

      //_movesHistory.append({"moveNum": count + 1})
      //_movesHistory.model.setProperty(count, "whiteMove", integration.w_move_lette)
     //_movesHistory.midel.setProperty(count, "blackMove", integration.b_move_lette)
    }
  }
}

