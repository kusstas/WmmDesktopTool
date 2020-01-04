import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Item {
    id: root

    Rectangle {
        height: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: Material.accentColor
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 15
        spacing: 10

        ComboBox {
            id: comboBox

            anchors.verticalCenter: parent.verticalCenter
            model: deviceMonitor.names

            onActivated: {
                deviceMonitor.selectedIndex = index;
            }

            Binding {
                target: comboBox
                property: "currentIndex"
                value: deviceMonitor.selectedIndex
            }
        }

        Button {
            id: connectButton

            width: 150
            enabled: device.portName.length > 0
            text: qsTr(device.connected ? "Disconnect" : "Connect")

            onClicked: {
                if (device.connected) {
                    if (!device.close()) {
                        // TODO: error
                    }
                }
                else {
                    if (!device.open()) {
                        // TODO: error
                    }
                }
            }
        }
    }
}
