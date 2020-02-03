import QtQuick 2.0
import ThreeInARow 1.0

GridView {
    id: gridView

    cellHeight: height / 4
    cellWidth: width / 4

    //flow: GridView.FlowTopToBottom
    interactive: false

    currentIndex: -1
    property int animationDuration: 2500
    property int appearanceAnimationDuration: 1000

    model: GameBoardModel {}

    delegate: Item {
        width: gridView.cellWidth
        height: gridView.cellHeight

        Rectangle {
            id: tile

            width: gridView.cellWidth < gridView.cellHeight ? gridView.cellWidth : gridView.cellHeight
            height: width
            radius: width * 0.5

            color: index == currentIndex ? Qt.darker(decoration) : decoration
            border.color: "black"
            border.width: 1

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (currentIndex != -1 && currentIndex != index) {
                        gridView.model.makeMove(currentIndex, index);
                        currentIndex = -1;
                    }
                    else if (currentIndex == index) {
                        currentIndex = -1
                    }
                    else {
                        currentIndex = index;
                    }
                }
            }
        }
    }
    move: Transition {
        NumberAnimation { properties: "y,x"; duration: animationDuration}
    }
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: appearanceAnimationDuration}
        NumberAnimation { property: "scale"; from: 0; to: 1; duration: appearanceAnimationDuration}
        NumberAnimation { properties: "y"; duration: animationDuration}
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: appearanceAnimationDuration}
        NumberAnimation { property: "scale"; from: 1; to: 0; duration: appearanceAnimationDuration}
    }
    displaced: Transition {
        NumberAnimation { properties: "y"; duration: animationDuration}
    }
}

