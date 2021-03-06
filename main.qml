import QtQuick 2.14
import QtQuick.Window 2.14

Window {
    id: root

    visible: true
    width: 480
    height: 480

    GameBoard {
        id: gameBoard
        anchors.fill: parent
        anchors.topMargin: root.height / 6
    }

    Rectangle {
        id: button

        width: root.width / 4
        height: root.height / 10

        color: "orange"
        radius: 10

        anchors.left: parent.left
        anchors.leftMargin: root.width / 8
        anchors.top: parent.top
        anchors.topMargin: root.height / 25

        Text {
            anchors.centerIn: parent
            text: "Shuffle"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: gameBoard.model.shuffle()
        }
    }
}
