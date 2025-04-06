import QtQuick

Item {

    width: parent.width
    height: parent.height

    Rectangle {
        width: parent.width
        height: 1
        color: "red"
    }

    Rectangle {
        width: 1
        height: parent.height
        color: "red"
    }

    Rectangle {
        y: parent.height - 1
        width: parent.width
        height: 1
        color: "red"
    }

    Rectangle {
        x: parent.width - 1
        width: 1
        height: parent.height
        color: "red"
    }
}
