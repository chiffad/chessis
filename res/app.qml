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

  ListModel
  {
    id: _figureModel

    ListElement{ name:'R'; xCoord: 0; yCoord: 0; visible: true }
    ListElement{ name:'H'; xCoord: 1; yCoord: 0; visible: true }
    ListElement{ name:'E'; xCoord: 2; yCoord: 0; visible: true }
    ListElement{ name:'Q'; xCoord: 3; yCoord: 0; visible: true }
    ListElement{ name:'K'; xCoord: 4; yCoord: 0; visible: true }
    ListElement{ name:'E'; xCoord: 5; yCoord: 0; visible: true }
    ListElement{ name:'H'; xCoord: 6; yCoord: 0; visible: true }
    ListElement{ name:'R'; xCoord: 7; yCoord: 0; visible: true }
    ListElement{ name:'P'; xCoord: 0; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 1; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 2; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 3; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 4; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 5; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 6; yCoord: 1; visible: true }
    ListElement{ name:'P'; xCoord: 7; yCoord: 1; visible: true }

    ListElement{ name:'w_p'; xCoord: 0; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 1; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 2; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 3; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 4; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 5; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 6; yCoord: 6; visible: true }
    ListElement{ name:'w_p'; xCoord: 7; yCoord: 6; visible: true }
    ListElement{ name:'w_r'; xCoord: 0; yCoord: 7; visible: true }
    ListElement{ name:'w_h'; xCoord: 1; yCoord: 7; visible: true }
    ListElement{ name:'w_e'; xCoord: 2; yCoord: 7; visible: true }
    ListElement{ name:'w_q'; xCoord: 3; yCoord: 7; visible: true }
    ListElement{ name:'w_k'; xCoord: 4; yCoord: 7; visible: true }
    ListElement{ name:'w_e'; xCoord: 5; yCoord: 7; visible: true }
    ListElement{ name:'w_h'; xCoord: 6; yCoord: 7; visible: true }
    ListElement{ name:'w_r'; xCoord: 7; yCoord: 7; visible: true }
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

    model: _figureModel

    Image
    {
      id: _delegateFigure
      Drag.active: _dragArea.drag.active

      width: cELL_SIZE
      height: width

      x: xCoord * cELL_SIZE
      y: yCoord * cELL_SIZE

      source: 'img/' + name +'.png'

      MouseArea
      {
        id: _dragArea

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        drag.minimumX: 0
        drag.minimumY: 0
        drag.maximumX: bOARD_SIZE - cELL_SIZE
        drag.maximumY: bOARD_SIZE - cELL_SIZE
        drag.target: parent

        onPressed:
        {
          integration.move(integration.correct_img_coord(parent.x) / cELL_SIZE,
                           integration.correct_img_coord(parent.y) / cELL_SIZE)
        }

        onReleased:
        {
          if(integration.move(integration.correct_img_coord(parent.x) / cELL_SIZE,
                              integration.correct_img_coord(parent.y) / cELL_SIZE))
          {
            xCoord: integration.correct_img_coord(parent.x) / cELL_SIZE
            yCoord: integration.correct_img_coord(parent.x) / cELL_SIZE

            parent.x = integration.correct_img_coord(parent.x)
            parent.y = integration.correct_img_coord(parent.y)
          }

          else
          {
            parent.x = xCoord * cELL_SIZE
            parent.y = yCoord * cELL_SIZE
          }
        }
      }
    }
  }
}
