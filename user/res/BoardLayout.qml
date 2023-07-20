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

      model: FigureModel

      delegate: Figure {
        id: figure

        img_type: figure_name
        size: board.cell_size
        x: x_coord * board.cell_size
        y: y_coord * board.cell_size
        visible: figure_visible

        Drag.active: drag_area.drag.active

        MouseArea {
          id: drag_area

          property int x1
          property int y1

          Drag.active: true
          drag.target: parent
          anchors.fill: parent
          drag.minimumX: 0
          drag.minimumY: 0
          enabled: figure_name != "hilight"

          onPressed: {
            figure.z = Constants.active_figure_z
            drag_area.x1 = parent.x
            drag_area.y1 = parent.y
          }

          onReleased: {
            figure.z = Constants.passive_figure_z
            FigureModel.run_command("move", x1, y1, parent.x, parent.y)
          }
        }
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

    visible: FigureModel.is_check_mate

    MouseArea {
      anchors.fill: parent
      onClicked: parent.visible = false
    }
  }
}
