import QtQuick 2.5

Rectangle
{
  property alias text: _text.text
  height: 20
  color: parent.color

  Text
  {
    id: _text
    anchors.left: parent.left
  }
}
