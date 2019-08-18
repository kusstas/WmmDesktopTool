import QtQuick 2.13
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    visible: true
    width: 1024
    height: 720
    title: qsTr("Wmm Desktop Tool")

    Material.theme: Material.Dark
    Material.accent: Material.Purple

    ColumnLayout {
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.fill: parent

        DeviceControlTools {
            Layout.fillWidth: true
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            LogContainer {
                anchors.fill: parent
            }

            MouseArea {
                id: busyContent

                anchors.fill: parent
                enabled: facade.device.busy
                opacity: enabled

                Rectangle {
                    anchors.fill: parent
                    color: Material.backgroundColor
                    opacity: 0.5
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: busyContent.opacity
                }

                Behavior on opacity {
                    SmoothedAnimation {
                        velocity: 100
                        easing.type: Easing.InQuart
                    }
                }
            }
        }

        DeviceConnectionBar {
            Layout.fillWidth: true
        }
    }

    WmmMessageDialog {
        id: messageDialog
    }

    Connections {
        target: facade

        onShowMessage: {
            messageDialog.show(title, message)
        }
    }
}
