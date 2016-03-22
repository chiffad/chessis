import QtQuick 2.2
import QtQuick.Controls 1.4
import Integration 1.0

Item
{
  id: root
  width: cELL_NUMERATION_SIZE + bOARD_SIZE + mENU_SIZE
  height: bOARD_SIZE

  readonly property int bOARD_SIZE: 560
  readonly property int mENU_SIZE: 200
  readonly property int cELL_SIZE: bOARD_SIZE / 8
  readonly property int cELL_NUMERATION_SIZE: 5

  IntegrationClass
  {
    id: integration
    move_turn_color: "white"
    w_move_in_letter: ""
    b_move_in_letter: ""
  }

  Image
  {
    id: _board
    z: -1

    anchors.left: _verticalCellNumeration.right
    anchors.bottom: _horizontalCellNumeration.top

    source: "img/board.png"
    width: bOARD_SIZE
    height: bOARD_SIZE    
  }

  ListView
  {
    id: _verticalCellNumeration
    model: 8

    delegate: Rectangle
    {
      anchors.left: parent.left
      width: cELL_NUMERATION_SIZE
      height: cELL_SIZE

      Text
      {
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 3
        text: _verticalCellNumeration.currentIndex
      }
    }
  }

  ListView
  {
    id: _horizontalCellNumeration
    orientation: ListView.Horizontal
    model: 8

    delegate: Rectangle
    {
      anchors.left: parent.left
      anchors.bottom: parent.bottom
      width: cELL_NUMERATION_SIZE
      height: cELL_SIZE

      Text
      {
        anchors.verticalCenter: parent.verticalCenter
        font.pointSize: 3
        text: integration.letter_return(_horizontalCellNumeration.currentIndex)
      }
    }
  }

  MenuLayout
  {
    width: mENU_SIZE
    height: parent.height
    anchors.left: _board.right

    Button
    {
      id: _back_move_button
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.topMargin: parent.bORDER_WIDTH
      anchors.leftMargin: parent.bORDER_WIDTH

      width: 100
      height: parent.height/10

      text: "Back move"

      onClicked:
      {
        var FREE_FIELD = "."
        var addIndex = _figureModel.count + 1;
        var newIndex =_figureDelegate.asdasd //_figureDelegate.__getIndex(integration.prev_to_coord("x"), integration.prev_to_coord("y"))
        //_figureModel.get(newIndex).xCoord = integration.prev_from_coord("x")
        //_figureModel.get(newIndex).yCoord = integration.prev_from_coord("y")
        if(integration.figure_on_field_move_to() !== FREE_FIELD)
          _figureModel.append({"name": integration.figure_on_field_move_to(),
                               "xCoord": integration.prev_to_coord("x"),
                               "yCoord": integration.prev_to_coord("y"),
                               "isNotBeaten": true} )
        integration.back_move();
      }
    }
  }

  CellHighlight{id: _startCellHighlight}

  CellHighlight{id: _endCellHighlight}

  Repeater
  {
    id: _repeater
    model: FigureModel {id: _figureModel}

    delegate: FigureDelegate
    {
      id: _figureDelegate
      property int asdasd: 0
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
  }
}
