import QtQuick 2.0

Image
{
  width: cELL_SIZE
  height: width

  x: xCoord * cELL_SIZE
  y: yCoord * cELL_SIZE

  source: 'img/' + name +'.png'

  Drag.active: _dragArea.drag.active
}

