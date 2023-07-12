pragma Singleton

import QtQuick 2.12

QtObject {
  readonly property int board_z : 0
  readonly property int cell_hilight_z: 1
  readonly property int passive_figure_z: 2
  readonly property int active_figure_z: 3
  readonly property int check_mate_img_z: 4
  readonly property int enter_login_z: 5

  readonly property int board_layout_size: 560
  readonly property int menu_size: 200
  readonly property int radius: 5
}
