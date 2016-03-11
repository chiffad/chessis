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

  Repeater
  {
    id: _rep

    model: FigureModel {id: _figureModel}

    FigureDelegate
    {
      id: _figureDelegate

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
          integration.move(integration.correct_img_coord(parent.x),
                           integration.correct_img_coord(parent.y))
        }

        onReleased:
        {
          _figureDelegate.isFreeField = integration.is_free_field(integration.correct_img_coord(parent.x),
                                                                  integration.correct_img_coord(parent.y))

          if(integration.move(integration.correct_img_coord(parent.x),
                              integration.correct_img_coord(parent.y)))
          {
            if(!_figureDelegate.isFreeField)
            {
              for(_figureDelegate.indexFigureOnFeeld = 0;
                  !integration.is_the_same_coord(_figureModel.get(indexFigureOnFeeld).xCoord,
                                                 _figureModel.get(indexFigureOnFeeld).yCoord,
                                                 integration.correct_figure_coord(parent.x),
                                                 integration.correct_figure_coord(parent.y));
                  ++_figureDelegate.indexFigureOnFeeld);

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
