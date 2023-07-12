import QtQuick 2.12

import Constants 1.0

Image {
  id: root

  property int size
  readonly property double cell_size: root.size / root.cells_count
  readonly property int cell_numeration_size: 12
  readonly property int cells_count: 8

  width: root.size
  height: width
  z: Constants.board_z
  source: "qrc:/res/img/board.png"

  Repeater {
    model: root.cells_count

    Text {
      y: root.cell_size * index
      font.bold: true
      font.pointSize: root.cell_numeration_size
      text: root.cells_count - index
      opacity: 0.3
    }
  }

  Repeater {
    model: root.cells_count

    Text {
      x: root.cell_size * (index + 1) - root.cell_numeration_size
      y: root.size - 2 * root.cell_numeration_size

      font.bold: true
      font.pointSize: root.cell_numeration_size
      text: "a" + index
      opacity: 0.3
    }
  }

  Component.onCompleted: FigureModel.set_cell_size(root.cell_size, root.cell_size)
}
