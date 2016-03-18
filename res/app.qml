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
    z: -1
    source: "img/board.png"
    width: bOARD_SIZE
    height: bOARD_SIZE    
  }

  CellHighlight{id: _startCellHighlight}

  CellHighlight{id: _endCellHighlight}

  Repeater
  {
    model: FigureModel {id: _figureModel}

    FigureDelegate
    {
      id: _figure

      x: xCoord * cELL_SIZE
      y: yCoord * cELL_SIZE

      source: 'img/' + name +'.png'

      visible: isNotBeaten

      /*onBackMoveChanges
      {


      }*/

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
          _figure.z = 2

          integration.move(parent.x, parent.y)

          _startCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
          _startCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
          _startCellHighlight.visible = true
        }

        onReleased:
        {
          _figure.z = 1

          _endCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
          _endCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
          _endCellHighlight.visible = true

          _figure.isFreeField = integration.is_free_field(parent.x, parent.y)

          if(integration.move(parent.x, parent.y))
          {   
            if(!_figure.isFreeField)
            {   
              for(_figure.indexFigureOnFeeld = 0;
                 !integration.is_the_same_coord(_figureModel.get(indexFigureOnFeeld).xCoord,
                                                _figureModel.get(indexFigureOnFeeld).yCoord,
                                                parent.x,
                                                parent.y);
                 ++_figure.indexFigureOnFeeld);

              _figureModel.get(indexFigureOnFeeld).isNotBeaten = false
            }

            _figureModel.get(index).xCoord = integration.correct_figure_coord(parent.x)
            _figureModel.get(index).yCoord = integration.correct_figure_coord(parent.y)
          }

          parent.x = _figureModel.get(index).xCoord * cELL_SIZE
          parent.y = _figureModel.get(index).yCoord * cELL_SIZE
        }
      }
    }
  }
}
