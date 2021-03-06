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

  readonly property int fIG_ANGLE: -5
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
  readonly property int eNTER_LOGIN_Z: 5

  Login_input
  {
    id: _login_input

    visible: false
    anchors.centerIn: parent
    z: _root.eNTER_LOGIN_Z

    onData_entered:
    {
      if(login.length && pwd.length && FigureModel.set_login(login, pwd))
      {
        _error_visible = false
        _login_input.visible = false
        _menu_layout._text_inp.focus = true
      }
      else
      {
        _error_visible = true
        _error_text = "Wrong login/pwd!"
      }
    }
  }

  Connections
  {
    target: FigureModel

    onEnter_login:
    {
      _login_input.visible = true
      _login_input._error_visible = true
      _login_input._error_text = error_text
    }
  }
    
  MenuLayout
  {
    id: _menu_layout

    height: parent.height
    anchors.left: _board.right
    anchors.right: parent.right

    _move_output_model: FigureModel.get_moves_history
    _move_turn_im_source: FigureModel.get_move_turn_color
    _command_field_model: FigureModel.get_commands_hist
    _command_field_current_ind: FigureModel.get_last_command_hist_ind
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
    opacity: 0.3
    z: cHECK_MATE_IMG_Z

    source: "qrc:/img/checkMate.png"

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
      cube_scale: figure_name == "hilight"? 1 : 0.8

      x: sTART_X + (x_coord * cELL_SIZE_X)
      y: sTART_Y + (y_coord * cELL_SIZE_Y)
      z: figure_name == "hilight"? cELL_HILIGHT_Z : pASSIVE_FIGURE_Z

      visible: figure_visible
      opacity: figure_name == "hilight" ? 0.5 : 1.0

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
        enabled: figure_name != "hilight"

        onPressed:
        {
          _figure_delegate.z = aCTIVE_FIGURE_Z
          _dragArea.x1 = parent.x
          _dragArea.y1 = parent.y
        }

        onReleased:
        {
          _figure_delegate.z = pASSIVE_FIGURE_Z
          FigureModel.run_command("move", x1 - _root.sTART_X, y1 - _root.sTART_Y,
                                          parent.x - _root.sTART_X, parent.y - _root.sTART_Y)
        }
      }
    }
  }
}
