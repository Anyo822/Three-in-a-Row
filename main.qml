import QtQuick 2.14
import QtQuick.Window 2.14

Window {
    id: root

    visible: true
    width: 980
    height: 980

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: root.height / 6

        color: "lightGray"

        GameBoard {
            id: gameBoard
            anchors.fill: parent
        }
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
            text: "Reset"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: gameBoard.model.shuffle()
        }
    }

    Rectangle {
        id: scoreText

        width: root.width / 4
        height: root.height / 10

        color: "green"
        radius: 10

        anchors.right: parent.right
        anchors.rightMargin: root.width / 8
        anchors.top: parent.top
        anchors.topMargin: root.height / 25

        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.height / 5
            anchors.right: parent.right
            anchors.rightMargin: parent.width / 4

            text: "Score: " + gameBoard.model.score
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.height * 3 / 5
            anchors.right: parent.right
            anchors.rightMargin: parent.width / 4

            text: "Moves: " + gameBoard.model.moves
        }
    }
}
