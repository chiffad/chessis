import QtQuick 2.0

Item
{
  id: root
  width: bOARD_SIZE
  height: bOARD_SIZE

  readonly property int cELL_NUMERATION_SIZE: 10

  Image
  {
    z: bOARD_Z

    source: "img/board.png"
    width: parent.width
    height: parent.height
  }

  Repeater
  {
    id: _horizontalCellNumeration
    model: 8

    Text
    {
      y: cELL_SIZE * index
      font.pointSize: cELL_NUMERATION_SIZE
      text: 8 - index
    }
  }

  Repeater
  {
    id: _verticalCellNumeration
    model: 8

    Text
    {
      x: cELL_SIZE * (index+1) - cELL_NUMERATION_SIZE
      y: bOARD_SIZE - 2*cELL_NUMERATION_SIZE

      font.pointSize: cELL_NUMERATION_SIZE
      text: FigureModel.letter_return(index)
    }
  }
}
