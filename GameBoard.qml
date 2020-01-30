import QtQuick 2.0
import ThreeInARow 1.0

GridView {
    id: root

    cellHeight: height / 8
    cellWidth: width / 8

    flow: GridView.FlowTopToBottom
    interactive: false

    currentIndex: -1

    model: GameBoardModel {}

    delegate: Item {
        width: root.cellWidth
        height: root.cellHeight

        Rectangle {
            id: tile

            width: root.cellWidth < root.cellHeight ? root.cellWidth : root.cellHeight
            height: width
            radius: width * 0.5

            color: index == currentIndex ? Qt.darker(decoration) : decoration
            border.color: "black"
            border.width: 1

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (currentIndex != -1 && currentIndex != index) {
                        root.model.moveMade(currentIndex, index);
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
    add: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 400}
        NumberAnimation { property: "scale"; from: 0; to: 1; duration: 400}
        NumberAnimation { properties: "x,y"; duration: 400}
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 400}
        NumberAnimation { property: "scale"; from: 1; to: 0; duration: 400}
    }
    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 400}
    }
}
