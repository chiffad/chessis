import QtQuick 2.5

Rectangle
{
  property alias text: _text.text

  height: 20

  Text
  {
    id: _text
    anchors.fill: parent
  }
}