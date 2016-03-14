import QtQuick 2.2
import Integration 1.0

Item
{
  id: root
  width: bOARD_SIZE //+ oUTPUT_SIZE
  height: bOARD_SIZE

  readonly property int bOARD_SIZE: 560
  readonly property int oUTPUT_SIZE: 200
  readonly property int cELL_SIZE: bOARD_SIZE / 8

  IntegrationClass
  {
    id: integration
  }

  Image
  {
    id: _board
    source: "img/board.JPG"
    width: bOARD_SIZE
    height: bOARD_SIZE    
  }

  CellHighlight{id: _startCellHighlight}

  CellHighlight{id: _endCellHighlight}

  Repeater
  {
    id: _rep

    model: FigureModel {id: _figureModel}

    FigureDelegate
    {
      id: _figure

      x: xCoord * cELL_SIZE
      y: yCoord * cELL_SIZE

      source: 'img/' + name +'.png'

      visible: isNotBeaten

      MouseArea
      {
        id: _dragArea

        anchors.fill: parent
        drag.minimumX: 0
        drag.minimumY: 0
        drag.target: parent

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed:
        { 
          _figure.z = 1

          integration.move(parent.x, parent.y)

          _startCellHighlightx = parent.x
          _startCellHighlighty = parent.y
        }

        onReleased:
        {
          _figure.z = 0

          _endCellHighlight.x = parent.x
          _endCellHighlight.y = parent.y

          _figure.isFreeField = integration.is_free_field(parent.x, parent.y)

          if(integration.move(parent.x, parent.y))
          {
            if(!_figure.isFreeField)
            {
              _figureModel.get(index).xCoord = integration.correct_figure_coord(parent.x)
              _figureModel.get(index).yCoord = integration.correct_figure_coord(parent.y)

                for(_figure.indexFigureOnFeeld = 0;
                  !integration.is_the_same_coord(_figureModel.get(indexFigureOnFeeld).xCoord,
                                                 _figureModel.get(indexFigureOnFeeld).yCoord,
                                                 parent.x,
                                                 parent.y);
                  ++_figure.indexFigureOnFeeld);

              _figureModel.get(indexFigureOnFeeld).isNotBeaten = false
            }
          }

          parent.x = _figureModel.get(index).xCoord * cELL_SIZE
          parent.y = _figureModel.get(index).yCoord * cELL_SIZE
        }
      }
    }
  }
}
