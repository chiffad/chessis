import QtQuick 2.12
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0

Item {
  id: root

  property alias error_text: error_text_field.text
  property alias error_visible: error_text_field.visible
  signal data_entered(string login, string pwd)

  height: parent.height/5
  width: parent.width/2

  Rectangle {
    id: root_rect

    height: parent.height
    width: parent.width

    color: "light grey"

    border.color: "black"
    border.width: 2
    radius: 5

    Rectangle {
      id: login_rect

      anchors.top: parent.top
      anchors.topMargin: parent.height / 5
      anchors.horizontalCenter: parent.horizontalCenter

      height: parent.height / 5
      width: parent.width - (parent.width / 10)

      border.color: "black"
      border.width: 1
      radius: 3
      color: "white"

      TextInput {
        id: login_input

        visible: parent.visible
        focus: visible
        wrapMode: TextInput.Wrap
        color: "black"

        font.pixelSize: parent.height - parent.height/3

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 2
        anchors.rightMargin: 2
        anchors.bottomMargin: 2
        maximumLength: 20

        clip: true

        onAccepted: {
          focus = false;
          pwd_rect.login = login_input.text;
          pwd_input.focus = true;
        }
      }
    }
    
    Rectangle {
      id: pwd_rect

      property string login
      
      anchors.top: login_rect.bottom
      anchors.topMargin: parent.height / 5
      anchors.horizontalCenter: parent.horizontalCenter

      height: parent.height / 5
      width: parent.width - (parent.width / 10)

      border.color: "black"
      border.width: 1
      radius: 3
      color: "white"

      TextInput {
        id: pwd_input

        visible: parent.visible
        focus: false
        wrapMode: TextInput.Wrap
        color: "black"

        font.pixelSize: parent.height - parent.height/3

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 2
        anchors.rightMargin: 2
        anchors.bottomMargin: 2
        maximumLength: 20

        clip: true

        onAccepted: {
          root.data_entered(pwd_rect.login, pwd_input.text);
          focus = false
          login_input.focus = root.visible
          text = "";
          login_input.text = "";
        }
      }
    }

    Text {
      id: error_text_field
      visible: parent.visible

      anchors.bottom: parent.bottom
      anchors.bottomMargin: 5
      anchors.horizontalCenter: parent.horizontalCenter

      font.pointSize: 9
      color: "red"
      opacity:0.7
    }
  }

  DropShadow {
    anchors.fill: root_rect
    horizontalOffset: 7
    verticalOffset: 7
    radius: root_rect.radius
    samples: 15
    spread: 0.3
    color: "#80000000"
    source: root_rect
  }
}
