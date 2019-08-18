import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

MouseArea {
    id: root

    property string name: ""
    property var dateTime: ""
    property string size: ""
    property bool selected: false

    readonly property color textColor: selected ? Material.accentColor : Material.foreground

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            spacing: 15

            Text {
                Layout.fillWidth: true
                color: textColor
                text: name
                elide: Text.ElideRight
            }

            Text {
                id: dateTimeText
                color: textColor
                text: dateTime.toLocaleString()
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 2
                Layout.topMargin: 2
                Layout.bottomMargin: 2
                color: "#474747"
            }

            Text {
                Layout.preferredWidth: 40
                color: textColor
                text: size
                horizontalAlignment: Text.AlignRight
                elide: Text.ElideRight
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color: "#474747"
        }
    }
}
