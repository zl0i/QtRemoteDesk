import QtQuick
import QtQuick.Controls
import QtMultimedia

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("FPDesk Reciver")

    Connections {
        target: reciver
        function onFrameUpdated() {
            _image.source = reciver.videoFrame
        }
        function onMouseMove(point) {
            _cursor.x = point.x
            _cursor.y = point.y
        }
    }

    Image {
        id: _image
        anchors.fill: parent
        source: reciver.videoFrame
    }

    Rectangle {
        id: _cursor
        width: 10
        height: 10
        radius: 5
        visible: reciver.isConnected
        opacity: 0.5
        color: "red"
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 60
        color: "#80000000"

        Row {
            anchors.centerIn: parent
            spacing: 20

            TextField {
                id: _codeField
                height: 20
                placeholderText: "Введите код"
            }

            Button {
                text: "Connect"
                enabled: !reciver.isConnected
                onClicked: reciver.connectToServer(_codeField.text)
            }

            Button {
                text: "Disconnect"
                enabled: reciver.isConnected
                onClicked: reciver.disconnectFromServer()
            }
        }
    }
}
