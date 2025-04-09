import QtQuick
import QtQuick.Controls
import QtMultimedia

ApplicationWindow {
    id: _app
    width: 640
    height: 480
    visible: true
    title: qsTr("FPDesk Receiver")

    Connections {
        target: receiver
        function onFrameUpdated() {
            _image.source = ""
            _image.source = "image://remoteimage/" + receiver.code
        }
    }

    Image {
        id: _image
        width: _app.width
        height: _app.height
        cache: false
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
                enabled: !receiver.isConnected
                onClicked: receiver.connectVideo(_codeField.text)
            }

            Button {
                text: "Disconnect"
                enabled: receiver.isConnected
                onClicked: receiver.disconnectFromServer()
            }

            Button {
                text: "Control"
                enabled: receiver.isConnected
                onClicked: receiver.connectEvent()
            }
        }
    }
}
