import QtQuick 2.0
import QtQuick.Controls 1.4

Rectangle
{
  color: "bisque"

  readonly property int bORDER_WIDTH: 2
  readonly property int mARGIN : 3

  border.width: bORDER_WIDTH
  border.color: "black"

  /*Button
  {
    id: _back_move_button
    anchors.left: parent.left
    anchors.top: parent.top
    anchors.topMargin: bORDER_WIDTH
    anchors.leftMargin: bORDER_WIDTH

    width: 100
    height: parent.height/10

    text: "Back move"

    onClicked:
    {
      var FREE_FIELD = "."
      var addIndex = _figureModel.count + 1;
      var newIndex = _figure.__getIndex(integration.prev_to_coord("x"), integration.prev_to_coord("y"))
      _figureModel.get(newIndex).xCoord = integration.prev_from_coord("x")
      _figureModel.get(newIndex).yCoord = integration.prev_from_coord("y")
      if(integration.figure_on_field_move_to() !== FREE_FIELD)
        _figureModel.append({"name": integration.figure_on_field_move_to(),
                             "xCoord": integration.prev_to_coord("x"),
                             "yCoord": integration.prev_to_coord("y"),
                             "isNotBeaten": true} )
      integration.back_move();
    }
  }*/

  /*MoveTurn
  {
    anchors.left: parent.left
    anchors.top: _back_move_button.bottom
    anchors.topMargin: mARGIN
    width: parent.width
    height: parent.height/8
  }*/

  /*MoveOutput
  {
    anchors.left: parent.left
    anchors.top: _moveTurn.bottom
    anchors.topMargin: mARGIN

    width: parent.width
    height: parent.height/4
  }*/
}

