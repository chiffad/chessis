import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import CubeRendering 1.0

Window
{
  id: root
  visible: true
  width: bOARD_SIZE + mENU_SIZE
  height: bOARD_SIZE

  maximumHeight: height
  minimumHeight: height
  maximumWidth: width
  minimumWidth: width

  readonly property int bOARD_SIZE: 560
  readonly property int mENU_SIZE: 200
  readonly property int cELL_SIZE: bOARD_SIZE / 8
  readonly property int bOARD_Z : 0
  readonly property int cELL_HILIGHT_Z: 1
  readonly property int pASSIVE_FIGURE_Z: 2
  readonly property int aCTIVE_FIGURE_Z: 3
  readonly property int cHECK_MATE_IMG_Z: 4

  MenuLayout
  {
    height: parent.height
    width: mENU_SIZE
    //anchors.left: _board.right
    anchors.right: parent.right
  }

  Image
  {
    id: _checkMate
    source: "img/checkMate.png"
    z: cHECK_MATE_IMG_Z
    width: _board.width
    height: _board.height / 2
    anchors.centerIn: _board
    visible: FigureModel.is_check_mate

    MouseArea
    {
      anchors.fill: parent
      onClicked: parent.visible = false
    }
  }

  Cube
  {
    id: _board

    width: bOARD_SIZE
    height: bOARD_SIZE
    anchors.left: parent.left
    z: bOARD_Z

    fig_type: "board"

    MouseArea
    {
      anchors.fill: parent
      onPressed: _board.pressEvent("board");
    }
  }

  Repeater
  {
    id: _repeater

    model: FigureModel

    delegate: Cube
    {
      id: _figure_delegate

      width: cELL_SIZE
      height: width

      fig_type: "cube"

     // x: x_coord * cELL_SIZE
     // y: y_coord * cELL_SIZE
      //z: figure_name == "hilight"? cELL_HILIGHT_Z : pASSIVE_FIGURE_Z

      visible: true//figure_visible
     // opacity: figure_name == "hilight" ? 0.5 : 1.0

      MouseArea
      {
        id: _dragArea

        property int x1
        property int y1

        Drag.active: true
        drag.target: _figure_delegate
        anchors.fill: parent
        drag.minimumX: 0
        drag.minimumY: 0
      //  enabled: figure_name != "hilight"

        onPressed:
        {
          _figure_delegate.pressEvent("pawn");
          _figure_delegate.z = aCTIVE_FIGURE_Z
          _dragArea.x1 = parent.x
          _dragArea.y1 = parent.y
        }

//        onReleased:
//        {
//          _figureDelegate.z = pASSIVE_FIGURE_Z
//          FigureModel.run_command(mOVE_WORD, x1, y1, parent.x, parent.y)
//        }
      }
    }
  }
}
