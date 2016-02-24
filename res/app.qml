import QtQuick 2.2
import Integration 1.0

Item
{

  id: root
  width: bOARD_SIZE //+ oUTPUT_SIZE
  height: bOARD_SIZE

  property int bOARD_SIZE: 560
  property int oUTPUT_SIZE: 200
  property int cELL_SIZE: bOARD_SIZE/8
  property int fIG_LEFT: 32
  property int cELL_NUM: 64

  IntegrationClass
  {
    id: integration
  }

  ListModel
  {
    id: figureModel

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
    id: board
    source: "img/board.JPG"
    width: bOARD_SIZE
    height: bOARD_SIZE
  }

  Repeater
  {
    id: rep
    model: figureModel

    delegate: Figure
    {
      id: delegateFigure

      width: cELL_SIZE
      height: width

      x: xCoord * cELL_SIZE
      y: yCoord * cELL_SIZE

      source: 'img/' + name +'.png'

      MouseArea
      {
        onPressed:
        {
          integration.move(delegateFigure.x, delegateFigure.y)
        }

        onReleased:
        {
          integration.move(delegateFigure.x, delegateFigure.y)
        }
      }
    }
  }
}
