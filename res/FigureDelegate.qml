import QtQuick 2.0

Image
{
  width: cELL_SIZE
  height: width
  z: figure_name == "hilight"? cELL_HILIGHT_Z : pASSIVE_FIGURE_Z
  opacity: figure_name == "hilight" ? 0.5 : 1.0

  Drag.active: _dragArea.drag.active
}
