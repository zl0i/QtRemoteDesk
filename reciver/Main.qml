import QtQuick
import QtQuick.Controls
import QtMultimedia

ApplicationWindow {
    id: _app
    width: 640
    height: 480
    visible: true
    title: qsTr("FPDesk Reciver")

    Connections {
        target: reciver
        function onFrameUpdated() {
            _image.source = ""
            _image.source = "image://remoteimage/" + reciver.code
        }
        function onMouseMove(point) {
            _cursor.x = point.x
            _cursor.y = point.y
        }
    }

    Image {
        id: _image
        width: _app.width
        height: _app.height
        cache: false
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
                onClicked: reciver.connectVideo(_codeField.text)
            }

            Button {
                text: "Disconnect"
                enabled: reciver.isConnected
                onClicked: reciver.disconnectFromServer()
            }

            Button {
                text: "Control"
                enabled: reciver.isConnected
                onClicked: reciver.connectEvent()
            }
        }
    }
}
