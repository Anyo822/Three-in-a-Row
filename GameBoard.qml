import QtQuick 2.14
import ThreeInARow 1.0

GridView {
    id: gridView

    cellHeight: height / model.boardWidth
    cellWidth: width / model.boardHeight

    flow: GridView.FlowTopToBottom
    interactive: false

    currentIndex: -1
    property int animationDuration: 1500
    property int appearanceAnimationDuration: 700
    property var itemFrom
    property var itemTo

    model: GameBoardModel {}

    delegate: Delegate {}

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

    Connections {
        target: gridView.model

        onWrongMove: {
            itemFrom = gridView.itemAtIndex(indexFrom);
            itemTo = gridView.itemAtIndex(indexTo);

            wrongMoveAnimation.start();
        }
    }

    GameOverDialog {
        id: messageDialog
    }

    WrongMoveAnimation {
        id: wrongMoveAnimation
        loops: 2
    }
}

