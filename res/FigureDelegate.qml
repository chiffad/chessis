import QtQuick 2.0

Image
{
  property bool __isFreeField
  property int indexFigureOnFeeld

  width: cELL_SIZE
  height: width
  z: 1

  Drag.active: _dragArea.drag.active

  function getIndex(x, y)
  {
    var correctX = integration.correct_figure_coord(x)
    var correctY = integration.correct_figure_coord(y)

    for(indexFigureOnFeeld = 0; indexFigureOnFeeld <= _figureModel.count; ++indexFigureOnFeeld)
    {
      if(_figureModel.get(indexFigureOnFeeld).xCoord == correctX
         && _figureModel.get(indexFigureOnFeeld).yCoord == correctY)
        break;
    }
    if(indexFigureOnFeeld > _figureModel.count)
      indexFigureOnFeeld = _figureModel.count;

    return indexFigureOnFeeld;
  }
}

