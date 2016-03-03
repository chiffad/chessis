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

    model: FigureModel {}
    delegate: FigureDelegate
    {
      id: _delegateFigure

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
          if(integration.move(integration.correct_img_coord(xCoord),
                              integration.correct_img_coord(yCoord)))
          {
            xCoord: integration.correct_img_coord(parent.x)
            yCoord: integration.correct_img_coord(parent.x)
          }

          //parent.x = xCoord * cELL_SIZE
          //parent.y = yCoord * cELL_SIZE
        }
      }
    }
  }
}
