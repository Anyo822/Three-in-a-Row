import QtQuick 2.14

SequentialAnimation {

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
