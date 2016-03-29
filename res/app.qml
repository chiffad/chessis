import QtQuick 2.2
import QtQuick.Controls 1.4
import Integration 1.0

Item
{
  id: root
  width: bOARD_SIZE + mENU_SIZE
  height: bOARD_SIZE

  readonly property int bOARD_SIZE: 560
  readonly property int mENU_SIZE: 200
  readonly property int cELL_SIZE: bOARD_SIZE / 8

  ListView {
      width: 200; height: 250

      model: FeModel
      delegate: Text { text: figure_name}
  }

  /*IntegrationClass
  {
    id: integration
    move_turn_color: "white"
    w_move_in_letter: ""
    b_move_in_letter: ""
  }

  //BoardInit{id: _board}

  MenuLayout
  {
    width: mENU_SIZE
    height: parent.height
    anchors.left: _board.right
  }

  //CellHighlight{id: _startCellHighlight}
  //CellHighlight{id: _endCellHighlight}

  Repeater
  {
    id: _repeater
    model: FigureModel {id: _figureModel}

    delegate: FigureDelegate
    {
      id: _figureDelegate

      x: xCoord * cELL_SIZE
      y: yCoord * cELL_SIZE

      source: 'img/' + model.name +'.png'

      visible: isNotBeaten

      MouseArea
      {
        id: _dragArea

        anchors.fill: parent
        drag.minimumX: 0
        drag.minimumY: 0
        drag.target: parent

        onPressed:
        {    
          _figureDelegate.z = 2

          integration.move(parent.x, parent.y)

          _startCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
          _startCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
          _startCellHighlight.visible = true
          _endCellHighlight.visible = false
        }

        onReleased:
        {
          _figureDelegate.z = 1

          _figureDelegate.__isFreeField = integration.is_free_field(parent.x, parent.y)

          if(integration.move(parent.x, parent.y))
          {
            _endCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
            _endCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
            _endCellHighlight.visible = true

            if(!_figureDelegate.__isFreeField)
              _figureModel.remove(__getIndex(parent.x, parent.y))

            _figureModel.get(index).xCoord = integration.correct_figure_coord(parent.x)
            _figureModel.get(index).yCoord = integration.correct_figure_coord(parent.y)
          }

          parent.x = _figureModel.get(index).xCoord * cELL_SIZE
          parent.y = _figureModel.get(index).yCoord * cELL_SIZE
        }
      }
    }
  }*/
}
