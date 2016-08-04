import QtQuick 2.0
import CubeRendering 1.0

Cube
{
  width: cELL_SIZE
  height: width

  fig_type: "cube"
  z: figure_name == "hilight"? cELL_HILIGHT_Z : pASSIVE_FIGURE_Z
  opacity: figure_name == "hilight" ? 0.5 : 1.0

  Drag.active: _dragArea.drag.active
}
