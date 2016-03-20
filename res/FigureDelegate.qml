import QtQuick 2.0

Image
{
  property bool __isFreeField
  property int indexFigureOnFeeld

  width: cELL_SIZE
  height: width
  z: 1

  Drag.active: _dragArea.drag.active

  function __getIndex(x, y)
  {
    if(x > 8 || y > 8)
    {
      x = integration.correct_figure_coord(x)
      y = integration.correct_figure_coord(y)
    }

    for(indexFigureOnFeeld = 0; indexFigureOnFeeld <= _figureModel.count; ++indexFigureOnFeeld)
    {
      if(_figureModel.get(indexFigureOnFeeld).xCoord == x
         && _figureModel.get(indexFigureOnFeeld).yCoord == y)
        break;
    }
    if(indexFigureOnFeeld > _figureModel.count)
      indexFigureOnFeeld = _figureModel.count;

    return indexFigureOnFeeld;
  }
}

