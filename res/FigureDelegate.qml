import QtQuick 2.0

Image
{
  property bool __isFreeField

  width: cELL_SIZE
  height: width
  z: 1

  Drag.active: _dragArea.drag.active


  function __getIndex(x, y)
  {
    var indexFigureOnFeeld = 1;
    while(indexFigureOnFeeld <= _figureModel.count)
    {
      if(_figureModel.get(indexFigureOnFeeld).xCoord == x
         && _figureModel.get(indexFigureOnFeeld).yCoord == y)
        break;

      ++indexFigureOnFeeld;
    }
    return indexFigureOnFeeld;
  }
}

