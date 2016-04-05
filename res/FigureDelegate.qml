import QtQuick 2.0

Image
{
  width: cELL_SIZE
  height: width
  z: pASSIVE_FIGURE_Z

  Drag.active: _dragArea.drag.active
}
