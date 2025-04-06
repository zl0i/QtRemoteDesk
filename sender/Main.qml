import QtQuick
import QtQuick.Controls.Fusion

import "./qml"

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("FPDesk Sender")

    Connections {
        target: streamer
        function onRemoteMouseMove(point) {
            _removeCursor.x = point.x
            _removeCursor.y = point.y
        }
        function onWaitConnect(code) {
            _dialog.accessCode = code
            _dialog.statusConnect = "wait_connect"
        }
    }

    Rectangle {
        id: _removeCursor
        z: 10000
        width: 10
        height: 10
        radius: 5
        visible: streamer.isConnected ?? false
        opacity: 0.5
        color: "blue"
    }

    Column {
        x: 20
        y: 20
        spacing: 20
        Button {
            text: "start"
            onClicked: function () {
                streamer.start()
                _dialog.statusConnect = "pending"
                _dialog.open()
            }
        }
        Button {
            text: "stop"
            onClicked: function () {
                streamer.stop()
            }
        }

        Row {
            height: 25
            spacing: 16

            property int counter: 0

            Button {
                text: "++"
                onClicked: () => parent.counter += 1
            }
            Label {
                id: _label
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                color: "#FFFFFF"
                text: parent.counter
            }
        }
        TextField {
            width: 200
            height: 30
        }
    }

    ListView {
        x: 250
        y: 20
        width: 200
        height: parent.height - 40
        clip: true
        model: 50
        delegate: Label {

            required property int index
            required property var modelData

            width: 200
            height: 20
            verticalAlignment: Text.AlignVCenter
            color: "#FFFFFF"
            text: index
            Rectangle {
                y: parent.height - 1
                width: parent.width
                height: 1
                color: "#C4C4C4"
            }
        }
    }

    RemoteControlDialog {
        id: _dialog
    }
}
