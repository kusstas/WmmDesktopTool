import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.platform 1.1

import com.svarmed.backend 1.0

ColumnLayout {
    id: root

    readonly property Device device: facade.device
    property string selectedMonth: ""
    property int selectedItems: 0

    function unselectAll() {
        selectedItems = device.unselectAll()
        var contentY = logsInfoList.contentY
        logsInfoList.model = []
        logsInfoList.model = Qt.binding(function () { return device.selectedLogsInfo })
        logsInfoList.contentY = contentY
    }

    enabled: device.opened && !device.busy

    RowLayout {
        Layout.fillWidth: true

        Button {
            Layout.fillWidth: true
            text: qsTr("Logs info")
            enabled: device.logsMonths.length === 0

            onClicked: {
                device.getLogsMonths();
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Load all")

            onClicked: {
                device.loadAllLogs()
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Load all on page(%1)").arg(device.selectedLogsInfo.length)
            enabled: device.selectedLogsInfo.length > 0

            onClicked: {
                device.loadLogsByMonth(selectedMonth)
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Load selected(%1)").arg(selectedItems)
            enabled: selectedItems > 0

            onClicked: {
                device.loadSelectedLogs()
                unselectAll()
            }
        }

        Button {
            Layout.fillWidth: true
            text: qsTr("Unselect all")
            enabled: selectedItems > 0

            onClicked: {
                unselectAll()
            }
        }
    }

    RowLayout {
        Layout.leftMargin: 5
        Layout.rightMargin: 5
        spacing: 15

        Label {
            text: qsTr("Log folder:")
            color: Material.accentColor
            font.pixelSize: 16
        }

        TextField {
            id: folderField

            Layout.fillWidth: true

            onTextEdited: {
                folderDialog.folder = "file://" + text
            }
        }

        Button {
            text: qsTr("Browse")

            onClicked: {
                folderDialog.open()
            }
        }

        Button {
            text: qsTr("Show")

            onClicked: {
                facade.openUrl(folderField.text)
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true

        Rectangle {
            color: "transparent"
            border.color: "#282828"
            border.width: 2
            radius: 2
            Layout.preferredWidth: 120
            Layout.fillHeight: true
            clip: true

            ListView {
                id: logsMonthsList
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5
                interactive: contentHeight > height

                model: device.logsMonths

                delegate: Button {
                    enabled: selectedMonth !== text
                    topInset: 0
                    bottomInset: 0
                    width: parent.width
                    height: 25
                    text: modelData

                    onClicked: {
                        selectedMonth = text
                        device.getLogsInfoByMonth(selectedMonth)
                    }
                }

                onModelChanged: {
                    selectedMonth = ""
                    selectedItems = 0
                }
            }
        }

        Rectangle {
            color: "transparent"
            border.color: "#282828"
            border.width: 2
            radius: 2
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: logsInfoList
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5
                interactive: contentHeight > height
                clip: true

                model: device.selectedLogsInfo

                delegate: LogInfoElement {
                    width: parent.width
                    height: 25

                    name: modelData.file
                    dateTime: modelData.modified
                    size: modelData.sizeText
                    selected: device.isSelected(modelData)

                    onClicked: {
                        selected = !selected
                        if (selected) {
                            selectedItems = device.select(modelData)
                        }
                        else {
                            selectedItems = device.unselect(modelData)
                        }
                    }
                }
            }
        }
    }

    ApplicationWindow {
        id: deviceProgressWindow

        property string text: ""
        property string ett: ""
        property real progress: 0
        property int progressPercent: progress * 100

        flags: Qt.Dialog
        minimumWidth: 480
        minimumHeight: columnDeviceProgressWindow.height + 20
        maximumWidth: minimumWidth
        maximumHeight: minimumHeight

        Material.theme: Material.Dark
        Material.accent: Material.Purple
        modality: Qt.WindowModal

        ColumnLayout {
            id: columnDeviceProgressWindow

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            spacing: 10

            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                color: Material.foreground
                font.bold: true
                font.pixelSize: 16
                text: deviceProgressWindow.ett

                Text {
                    anchors.right: parent.right
                    color: Material.foreground
                    font.bold: true
                    font.pixelSize: 16
                    text: deviceProgressWindow.progressPercent + "%"
                }
            }

            ProgressBar {
                Layout.fillWidth: true
                value: deviceProgressWindow.progress
            }

            Label {
                Layout.fillWidth: true
                color: Material.foreground
                font.bold: true
                font.pixelSize: 16
                text: deviceProgressWindow.text
            }

            Button {
                text: qsTr("Abort")
                Layout.fillWidth: true

                onClicked: {
                    device.abortLoadLogs()
                }
            }
        }
    }

    FolderDialog {
        id: folderDialog

        onAccepted: {
            folderField.text = folder.toString().replace("file://", "")
        }

        Component.onCompleted: {
            folder = "file://" + defaultLogDir
            folderField.text = defaultLogDir
        }

        onFolderChanged: {
            device.setFolderLogLoad(folder)
        }
    }

    Connections {
        target: device

        onTextProgressChanged: {
            deviceProgressWindow.text = text
            deviceProgressWindow.visible = true
        }

        onProgressChanged: {
            deviceProgressWindow.progress = progress
            deviceProgressWindow.visible = true
        }

        onEttChanged: {
            deviceProgressWindow.ett = ett
            deviceProgressWindow.visible = true
        }

        onProgressDialog: {
            deviceProgressWindow.title = title
            deviceProgressWindow.visible = title.length > 0
        }
    }
}
