import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import com.svarmed.backend 1.0

RowLayout {
    id: root

    readonly property Device device: facade.device
    enabled: device.opened && !device.busy

    Button {
        Layout.fillWidth: true
        text: qsTr("User info")

        onClicked: {
            var userInfo = device.getUserInfo()
            facade.showMessage(text, userInfo)
        }
    }

    Button {
        Layout.fillWidth: true
        text: qsTr("Set user info")

        onClicked: {
            inputUserDataWindow.visible = true
        }
    }

    Button {
        Layout.fillWidth: true
        text: qsTr("Version info")

        onClicked: {
            var version = device.getVersionInfo()
            facade.showMessage(text, version)
        }
    }

    Button {
        Layout.fillWidth: true
        text: qsTr("Timestamp")

        onClicked: {
            var time = device.getTimestamp()
            facade.showMessage(text, time)
        }
    }

    Button {
        Layout.fillWidth: true
        text: qsTr("Sync time")

        onClicked: {
            device.syncDateTime()
            var time = device.getTimestamp()
            facade.showMessage(text, time)
        }
    }

    Button {
        Layout.fillWidth: true
        text: qsTr("Restart")

        onClicked: {
            device.restart()
        }
    }

    ApplicationWindow {
        id: inputUserDataWindow

        title: qsTr("Enter user info")
        flags: Qt.Dialog
        minimumWidth: 480
        minimumHeight: column.height + 20
        maximumWidth: minimumWidth
        maximumHeight: minimumHeight

        Material.theme: Material.Dark
        Material.accent: Material.Purple
        modality: Qt.WindowModal

        ColumnLayout {
            id: column

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 10

            TextField {
                id: inputUserDataField
                Layout.fillWidth: true

                placeholderText: qsTr("Some user info")
                verticalAlignment: Text.AlignTop
                wrapMode: Text.WordWrap
                maximumLength: 100
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Save")

                onClicked: {
                    var text = inputUserDataField.text
                    inputUserDataWindow.close()
                    device.setUserInfo(text)
                }
            }
        }

        onClosing: {
            inputUserDataField.text = ""
        }
    }
}
