import QtQuick 2.0

Image
{
  Drag.active: dragArea.drag.active

  MouseArea
  {
    id: dragArea

    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    drag.minimumX: 0
    drag.minimumY: 0
    drag.maximumX: bOARD_SIZE - cELL_SIZE
    drag.maximumY: bOARD_SIZE - cELL_SIZE
    drag.target: parent
   }
}
