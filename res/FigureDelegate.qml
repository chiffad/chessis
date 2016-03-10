import QtQuick 2.0

Image
{
  width: cELL_SIZE
  height: width

  property bool isFreeField
  property int indexFigureOnFeeld

  Drag.active: _dragArea.drag.active
}

