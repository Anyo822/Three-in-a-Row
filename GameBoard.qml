import QtQuick 2.14
import ThreeInARow 1.0
import QtQuick.Dialogs 1.1

GridView {
    id: gridView

    cellHeight: height / model.boardHeight
    cellWidth: width / model.boardWidth

    flow: GridView.FlowTopToBottom
    interactive: false

    currentIndex: -1
    property int animationDuration: 1500
    property int appearanceAnimationDuration: 700
    property var itemFrom
    property var itemTo

    model: GameBoardModel {}

    delegate: Item {
        width: gridView.cellWidth
        height: gridView.cellHeight

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
    }

    Connections {
        target: gridView.model

        onWrongMove: {
            itemFrom = gridView.itemAtIndex(indexFrom);
            itemTo = gridView.itemAtIndex(indexTo);

            wrongMoveAnimation.start();
        }
    }

    SequentialAnimation {
        id: wrongMoveAnimation
        loops: 2

        NumberAnimation {
            targets: [itemFrom, itemTo]
            property: "scale"
            to: 0.7
            easing.type: "InOutQuad"
            easing.overshoot: 100
        }
        NumberAnimation {
            targets: [itemFrom, itemTo]
            property: "scale"
            to: 1
            easing.type: "InOutQuad"
            easing.overshoot: 100
        }
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
            NumberAnimation { properties: "y"; from: 0; duration: animationDuration}
        }
        onRunningChanged: {
            if (!running) {
                if (gridView.model.matchCheck()) {
                    gridView.model.removeMarkedTiles();
                }
                if (gridView.model.gameOverCheck()) {
                    messageDialog.open();
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

    MessageDialog {
        id: messageDialog
        title: "May I have your attention please"
        text: "Game Over, restart?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: gridView.model.shuffle();
        onNo: Qt.quit();
    }
}

