import QtQuick 2.12
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.15

import "login"

Dialog {        	
  id: root
  
  modal: true
  closePolicy: Popup.NoAutoClose	
  
  Connections {
    target: LoginInputController

    function onEnter_login() {
      root.open()
    }
  }

  function login_pwd_set() {
    root.close()
    LoginInputController.set_login(login_input.text, pwd_input.text);
    login_input.focus = true
    pwd_input.text = "";
    login_input.text = "";          
  }

  background: Rectangle {
    id: root_rect

    height: parent.height
    width: parent.width

    color: "light grey"

    border.color: "black"
    border.width: 2
    radius: 5
  }

  ColumnLayout {
    width: parent.width
    spacing: 5

    TextInputBox {
      id: login_input

      Layout.alignment: Qt.AlignHCenter      
      Layout.preferredWidth: parent.width
      Layout.preferredHeight: root.height / 5
      
      focus: true
      onAccepted: pwd_input.focus = true;
    }

    TextInputBox {
      id: pwd_input
      
      Layout.alignment: Qt.AlignHCenter
      Layout.preferredWidth: login_input.width
      Layout.preferredHeight: login_input.height

      onAccepted: root.login_pwd_set()    
    }
 
    Text {
      id: error_text_field
      visible: error_text_field.text.length != 0

      Layout.alignment: Qt.AlignHCenter

      text: LoginInputController.error_message 
      font.pointSize: 9
      color: "red"
      opacity: 0.7
    }

    DialogButtonBox {
      id: ok_button

      Layout.preferredWidth: login_input.width / 2
      Layout.preferredHeight: login_input.height

      standardButtons: DialogButtonBox.Ok
      Layout.alignment: Qt.AlignRight
      alignment: Qt.AlignHCenter
      background: Rectangle { visible: false }
      onAccepted: root.login_pwd_set()
    }
  }
}
