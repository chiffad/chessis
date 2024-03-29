import QtQuick 2.12
import Constants 1.0

Item {
  id: root
  property string img_type: ""
  property alias size: root.width
  property bool hilight_type: false

  height: width  
  Drag.active: drag_area.drag.active 

  Loader {
    id: loader
    active: root.visible && root.img_type
    anchors.fill: parent
    sourceComponent: Image {
      anchors.fill: parent
      source: "qrc:/res/img/" + root.img_type + ".png"
      Drag.active: drag_area.drag.active 
    }
  }

  MouseArea {
    id: drag_area

    property int x1
    property int y1

    anchors.fill: parent
    drag.target: parent

    onPressed: {
      drag_area.x1 = parent.x
      drag_area.y1 = parent.y
    }
    onReleased: BoardController.move(x1, y1, parent.x, parent.y)
  }

  states: [ 
    State {
      name: "figure"; when: !root.hilight_type
      PropertyChanges { 
        target: root
        opacity: 1 
        z: drag_area.drag.active ? Constants.active_figure_z : Constants.passive_figure_z
      }
      PropertyChanges { target: drag_area; enabled: true }
    },
    State {
      name: "hilight"; when: root.hilight_type
      PropertyChanges { 
        target: root 
        opacity: 0.5
        z: Constants.cell_hilight_z 
      }
      PropertyChanges { target: drag_area; enabled: false }
    }
  ]
}