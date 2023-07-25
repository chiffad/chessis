import QtQuick 2.12

import Constants 1.0

Item {
  id: root

  width: Constants.board_layout_size
  height: width

  Board {
    id: board

    anchors.centerIn: root
    size: root.width * 0.8 

    Repeater {
      id: board_obj

      model: FiguresModel

      delegate: Figure {
        id: figure

        hilight_type: figure_name === "hilight"
        img_type: figure_name
        size: board.cell_size
        x: figure_x * board.cell_size
        y: figure_y * board.cell_size
        visible: figure_visible
      }
    }
  }

  Image {
    id: check_mate

    width: board.width
    height: board.height / 2
    anchors.centerIn: parent
    opacity: 0.3

    source: "qrc:/res/img/checkMate.png"

    visible: BoardController.is_check_mate

    MouseArea {
      anchors.fill: parent
      onClicked: parent.visible = false
    }
  }
}
