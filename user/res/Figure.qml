import QtQuick 2.12
import Constants 1.0

Image {
  id: root
  property string img_type

  width: cELL_SIZE
  height: width
  z: root.img_type === "hilight" ? Constants.cell_hilight_z : Constants.passive_figure_z
  opacity: root.img_type === "hilight" ? 0.5 : 1.0
  source: "qrc:/res/img/" + root.img_type + ".png"
}
