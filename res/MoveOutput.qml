import QtQuick 2.5
import QtQml.Models 2.2

Rectangle
{
  property alias model: _grid.model
  property alias delegate: _grid.delegate

  readonly property int eLEMENT_HEIGHT: 20

  anchors.topMargin: border.width

  border.color: "black"

  GridView
  {
    id: _grid

    anchors.fill: parent
    anchors.margins: parent.border.width
    clip: true

    cellWidth: parent.width / 2 - 2
    cellHeight: eLEMENT_HEIGHT
  }
}
