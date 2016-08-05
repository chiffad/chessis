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
    anchors.left: _board.right
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
    visible: false//FigureModel.is_check_mate

    MouseArea
    {
      anchors.fill: parent
      onClicked: parent.visible = false
    }
  }

  BoardInit
  {
    id: _board
    width: bOARD_SIZE
    height: bOARD_SIZE
    _board.pressEvent();
  }

  Repeater
  {
    id: _repeater
    model: FigureModel

    delegate: FigureDelegate
    {
      id: _figureDelegate

      //x: x_coord * cELL_SIZE
      //y: y_coord * cELL_SIZE
      visible: true//figure_visible

      MouseArea
      {
        id: _dragArea

        property int x1
        property int y1
        drag.target: parent
        anchors.fill: parent
        drag.minimumX: 0
        drag.minimumY: 0
        enabled: figure_name != "hilight"

        onPressed:
        {
           _figure_delegate.pressEvent();
//          _figureDelegate.z = aCTIVE_FIGURE_Z
//          _dragArea.x1 = parent.x
//          _dragArea.y1 = parent.y
        }
  /*
        onReleased:
        {
          _figureDelegate.z = pASSIVE_FIGURE_Z
          FigureModel.run_command(mOVE_WORD, x1, y1, parent.x, parent.y)
        }*/
      }
    }
  }
}
