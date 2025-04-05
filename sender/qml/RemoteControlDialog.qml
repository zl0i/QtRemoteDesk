import QtQuick
import QtQuick.Controls

Dialog {
    id: _dialog

    property string statusConnect: "pending"
    property string accessCode: ""

    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2
    width: 300
    height: 200

    background: Rectangle {
        radius: 10
        color: "#1A1A1A"
    }

    contentItem: Item {

        Column {
            x: 20
            y: 20
            width: parent.width - 40
            Label {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                visible: _dialog.statusConnect == "pending"
                text: "Устанавливается соединение с сервером"
            }
            Column {
                width: parent.width
                visible: _dialog.statusConnect == "wait_connect"
                spacing: 8
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 16
                    text: "Сообщите этот код Технической Поддержке"
                }
                Label {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        pixelSize: 24
                        weight: Font.Bold
                        letterSpacing: 2
                    }
                    text: _dialog.accessCode
                }
            }
        }
    }
}
