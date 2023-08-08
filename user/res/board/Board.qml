import QtQuick 2.12

import Constants 1.0

Item {
  id: root
  property int size
  readonly property double cell_size: root.size / root.cells_count
  readonly property int cell_numeration_size: 12
  readonly property int cells_count: 8
  width: root.size
  height: width

  Image {
    id: board_img
    anchors.fill: parent
    source: "qrc:/res/img/board.png"
  }

  Repeater {
    model: root.cells_count

    Text {
      y: root.cell_size * index
      font.bold: true
      font.pointSize: root.cell_numeration_size
      opacity: 0.3
      text: BoardController.playing_white ? root.cells_count - index : index + 1        
    }
  }

  Repeater {
    model: root.cells_count

    Text {
      x: root.cell_size * (index + 1) - root.cell_numeration_size
      y: root.size - 2 * root.cell_numeration_size

      font.bold: true
      font.pointSize: root.cell_numeration_size
      opacity: 0.3
      text: String.fromCharCode(BoardController.playing_white ? 97 + index : 104 - index) //'a' + index or 'h' - index
                                          
    }
  }

  Component.onCompleted: BoardController.set_cell_size(root.cell_size, root.cell_size)
}
