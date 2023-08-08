import QtQuick 2.12

Board {
  id: root

  Repeater {
    id: board_obj
    model: FiguresModel
    delegate: Figure {            
      hilight_type: figure_name === "hilight"
      img_type: figure_name
      size: board.cell_size
      x: figure_x * board.cell_size
      y: figure_y * board.cell_size
      visible: figure_visible
    }
  }
}
