import QtQuick 2.13
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    id: root

    property string text: ""

    property var queue: []

    flags: Qt.Dialog
    minimumWidth: 480
    minimumHeight: columnMessageDialog.height + 20
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    Material.theme: Material.Dark
    Material.accent: Material.Purple
    modality: Qt.WindowModal

    function show(title, text) {
        if (visible) {
            queue.push({"title" : title, "text": text})
        }
        else {
            root.title = title
            root.text = text
            root.visible = true
        }
    }

    ColumnLayout {
        id: columnMessageDialog

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 10

        Label {
            Layout.fillWidth: true
            color: Material.foreground
            font.bold: true
            font.pixelSize: 16
            text: root.text
            wrapMode: Text.WordWrap
        }

        Button {
            text: qsTr("Ok")
            Layout.fillWidth: true

            onClicked: {
                if (queue.length > 0) {
                    var info = queue.shift()
                    root.title = info.title
                    root.text = info.text
                }
                else {
                    root.close()
                }
            }
        }
    }

    onClosing: {
        queue = []
    }
}
