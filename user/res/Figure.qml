import QtQuick 2.12
import Constants 1.0

Image {
  id: root
  property string imgType

  width: cELL_SIZE
  height: width
  z: root.imgType === "hilight" ? Constants.cell_hilight_z : Constants.passive_figure_z
  opacity: root.imgType === "hilight" ? 0.5 : 1.0
  source: "qrc:/res/img/" + root.imgType + ".png"
}
