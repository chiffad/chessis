import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import CubeRendering 1.0

Window
{
  id: _root

  visible: true
  width: bOARD_LAYOUT_SIZE + mENU_SIZE
  height: bOARD_LAYOUT_SIZE

  maximumHeight: height
  minimumHeight: height
  maximumWidth: width
  minimumWidth: width

  readonly property int bOARD_LAYOUT_SIZE: 560
  readonly property int mENU_SIZE: 200

  readonly property int fIG_ANGLE: -10
  readonly property int bOARD_ANGLE: -50
  readonly property double bOARD_COS_ANGLE: Math.cos((bOARD_ANGLE * Math.PI)/ 180)

  readonly property double bOARD_SIZE: bOARD_LAYOUT_SIZE * _board.cube_scale
  readonly property double cELL_SIZE_X: bOARD_SIZE / 8
  readonly property double cELL_SIZE_Y: bOARD_SIZE / 8 * bOARD_COS_ANGLE

  readonly property double sTART_X: (bOARD_LAYOUT_SIZE - bOARD_SIZE) / 2
  readonly property double sTART_Y: ((bOARD_LAYOUT_SIZE - (bOARD_SIZE * bOARD_COS_ANGLE)) - (bOARD_LAYOUT_SIZE * 0.1 * bOARD_COS_ANGLE)) / 2

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

    _move_output_model: FigureModel.get_moves_history
    _move_turn_im_source: FigureModel.get_move_turn_color
    _command_field_model: FigureModel.get_commands_hist
    _command_field_current_ind: FigureModel.get_last_elem_ind
    _connection_status_text: FigureModel.get_udp_connection_status

    onRun_command: FigureModel.run_command(command, num)
    onWork_with_file: FigureModel.path_to_file(path_to_file, is_from_file);
  }

  Image
  {
    id: _check_mate

    width: _board.width
    height: _board.height / 2
    anchors.centerIn: _board
    z: cHECK_MATE_IMG_Z

    source: "img/checkMate.png"

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

    width: bOARD_LAYOUT_SIZE
    height: bOARD_LAYOUT_SIZE
    anchors.left: parent.left
    z: bOARD_Z

    fig_type: "board"
    tilt_angle: _root.bOARD_ANGLE
    cube_scale: 0.8

    MouseArea
    {
      anchors.fill: parent
      onPressed:
      {

          console.log("x: ",mouseX)
          console.log("y: ",mouseY)
    /*      console.log("start y: ", _root.sTART_Y, ";y size: ", _root.cELL_SIZE_Y)
          console.log("start x: ", _root.sTART_X, ";x size: ", _root.cELL_SIZE_X)
          console.log("bOARD_COS_ANGLE: ", _root.bOARD_COS_ANGLE)
          console.log("bOARD_LAYOUT_SIZE: ", _root.bOARD_LAYOUT_SIZE)
          console.log("bOARD_SIZE: ", _root.bOARD_SIZE)
          console.log("ANGLE: ", _root.bOARD_COS_ANGLE)
          console.log("board:: ", _root.bOARD_COS_ANGLE * _root.bOARD_SIZE)
     */
      }
    }
  }

  Repeater
  {
    id: _figures

    model: FigureModel

    delegate: Cube
    {
      id: _figure_delegate

      width: cELL_SIZE_X
      height: cELL_SIZE_Y

      fig_type: figure_name
      tilt_angle: _root.fIG_ANGLE

      x: sTART_X + (x_coord * cELL_SIZE_X)
      y: sTART_Y + (y_coord * cELL_SIZE_Y)
      z: figure_name == "hilight"? cELL_HILIGHT_Z : pASSIVE_FIGURE_Z

      visible: figure_visible
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
          _figure_delegate.z = aCTIVE_FIGURE_Z
          _dragArea.x1 = parent.x
          _dragArea.y1 = parent.y
        }

        onReleased:
        {
          _figure_delegate.z = pASSIVE_FIGURE_Z
          console.log("x1: ",_dragArea.x1)
          console.log("y1: ",_dragArea.y1)
          console.log("x: ", parent.x)
          console.log("y: ", parent.y)

          FigureModel.run_command("move", x1 - START_X, y1 - START_Y, parent.x - START_X, parent.y - START_Y)
        }
      }
    }
  }
}
