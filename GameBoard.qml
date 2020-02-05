import QtQuick 2.14
import ThreeInARow 1.0

GridView {
    id: gridView

    cellHeight: height / 4
    cellWidth: width / 4

    flow: GridView.FlowTopToBottom
    interactive: false

    currentIndex: -1
    property int animationDuration: 1500
    property int appearanceAnimationDuration: 700

    model: GameBoardModel {} //run animation on signal

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


    Connections {
        target: gridView.model

        onWrongMove: {
            var itemFrom = gridView.itemAtIndex(indexFrom);
            var itemTo = gridView.itemAtIndex(indexTo);

            wrongMoveAnimation.targets = [itemFrom, itemTo];
            wrongMoveAnimation.start();
        }
    }

    NumberAnimation {
        id: wrongMoveAnimation

        properties: "scale"
        to: 0.1
        duration: 1500
    }

    move: Transition {
        NumberAnimation {
            properties: "y,x";
            duration: animationDuration
        }
        onRunningChanged: {
            if (!running) {
                gridView.model.removeMarkedTiles();
            }
        }
    }
    add: Transition {
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: appearanceAnimationDuration}
            NumberAnimation { property: "scale"; from: 0; to: 1; duration: appearanceAnimationDuration}
            NumberAnimation { properties: "y"; from: -200; duration: animationDuration}
        }
        onRunningChanged: {
            if (!running) {
                if (gridView.model.matchCheck()) {
                    gridView.model.removeMarkedTiles();
                }
            }
        }
    }
    remove: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: appearanceAnimationDuration}
        NumberAnimation { property: "scale"; from: 1; to: 0; duration: appearanceAnimationDuration}
    }
    displaced: Transition {
        NumberAnimation {
            properties: "y,x";
            duration: animationDuration
        }
    }

    //check transition on comletion animation call
}

