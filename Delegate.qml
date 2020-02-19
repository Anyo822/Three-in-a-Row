import QtQuick 2.14

Rectangle {
    id: tile

    width: gridView.cellWidth < gridView.cellHeight ? gridView.cellWidth : gridView.cellHeight
    height: width
    radius: width * 0.5

    color: index === currentIndex ? Qt.darker(decoration) : decoration
    border.color: "black"
    border.width: 1

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (currentIndex != -1 && currentIndex != index) {
                gridView.model.makeMove(currentIndex, index);
                currentIndex = -1;
            } else if (currentIndex == index) {
                currentIndex = -1
            } else {
                currentIndex = index;
            }
        }
    }
}
