import QtQuick 2.0

Image
{
  property bool isFreeField
  property int indexFigureOnFeeld

  width: cELL_SIZE
  height: width
  z: 1

  Drag.active: _dragArea.drag.active
}

