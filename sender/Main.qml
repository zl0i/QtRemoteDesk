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
    }

    RemoteControlDialog {
        id: _dialog
    }
}
