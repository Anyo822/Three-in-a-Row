import QtQuick 2.14
import QtQuick.Dialogs 1.1

MessageDialog {
    title: "May I have your attention please"
    text: "Game Over, restart?"
    standardButtons: StandardButton.Yes | StandardButton.No
    onYes: gridView.model.shuffle();
    onNo: Qt.quit();
}
