import QtQuick 2.0
import ThreeInARow 1.0

GridView {
    id: root

    cellHeight: height / 8
    cellWidth: width / 8

    //flow: GridView.FlowTopToBottom
    interactive: false

    model: GameBoardModel {}

    delegate: Rectangle {
        width: root.cellWidth
        height: root.cellHeight
        radius: 20

        color: decoration
        border.color: "black"
    }
}
