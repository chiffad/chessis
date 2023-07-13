import QtQuick 2.12

Rectangle {
  property alias text: connection_status_text.text
  height: 20
  color: parent.color

  Text {
    id: connection_status_text
    anchors.left: parent.left
  }
}
