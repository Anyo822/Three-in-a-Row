import QtQuick 2.0
import ThreeInARow 1.0

GridView {
    id: root

    property int selectedIndex: -1

    cellHeight: height / 8
    cellWidth: width / 8

    //flow: GridView.FlowTopToBottom
    interactive: false

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
                    selectedIndex: setIndex()
                }

                function setIndex() {
                    if(currentIndex != -1 && currentIndex != index) {
                        root.model.moveMade(currentIndex, index);
                        currentIndex = -1;
                    }
                    else {
                        currentIndex = index;
                    }
                }
            }
        }
    }

    currentIndex: -1
}
