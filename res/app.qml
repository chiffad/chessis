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

  IntegrationClass
  {
    id: integration
    move_turn_color: "white"
  }

  Image
  {
    id: _board
    z: -1
    source: "img/board.png"
    width: bOARD_SIZE
    height: bOARD_SIZE    
  }

  /*Menu
  {
    width: mENU_SIZE
    height: parent.height
    anchors.left: _figure.right
  }*/

  Button
  {
    anchors.left: _board.right
    anchors.top: parent.top

    width: 100
    height: 50

    text: "Back move"

    onClicked:
    {
      var FREE_FIELD = "."
      var addIndex = _figureModel.count + 1;
      var newIndex = _figure.__getIndex(integration.prev_to_coord("x"), integration.prev_to_coord("y"))
      _figureModel.get(newIndex).xCoord = integration.prev_from_coord("x")
      _figureModel.get(newIndex).yCoord = integration.prev_from_coord("y")
      if(integration.figure_on_field_move_to() !== FREE_FIELD)
        _figureModel.insert(addIndex, {"name": integration.figure_on_field_move_to(),
                                       "xCoord": integration.prev_to_coord("x"),
                                       "yCoord": integration.prev_to_coord("y"),
                                       "isNotBeaten": true} )
      integration.back_move();
    }
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

      MouseArea
      {
        id: _dragArea

        anchors.fill: parent
        drag.minimumX: 0
        drag.minimumY: 0
        drag.target: parent

        onPressed:
        {    
          _figure.z = 2

          integration.move(parent.x, parent.y)

          _startCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
          _startCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
          _startCellHighlight.visible = true
          _endCellHighlight.visible = false
        }

        onReleased:
        {
          _figure.z = 1

          _figure.__isFreeField = integration.is_free_field(parent.x, parent.y)

          if(integration.move(parent.x, parent.y))
          {
            _endCellHighlight.x = integration.correct_figure_coord(parent.x) * cELL_SIZE
            _endCellHighlight.y = integration.correct_figure_coord(parent.y) * cELL_SIZE
            _endCellHighlight.visible = true

            if(!_figure.__isFreeField)
              _figureModel.remove(__getIndex(parent.x, parent.y))

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
