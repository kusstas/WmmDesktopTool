import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Item {
    id: root

    readonly property color selectedColor: Material.accentColor
    readonly property color defaultColor: Material.background

    readonly property color selectedTextColor: Material.background
    readonly property color defaultTextColor: Material.accentColor

    readonly property int settingsIndex: 0
    readonly property int userPreferencesIndex: 1

    readonly property bool isSettingsSelected: selected === settingsIndex
    readonly property bool userPreferencesSelected: selected === userPreferencesIndex

    property int selected: settingsIndex

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: isSettingsSelected ? selectedColor : defaultColor

            Text {
                anchors.centerIn: parent
                text: qsTr("Settings")
                color: isSettingsSelected ? selectedTextColor : defaultTextColor
                font.pointSize: 12
            }

            MouseArea {
                anchors.fill: parent
                onClicked: selected = settingsIndex
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: userPreferencesSelected ? selectedColor : defaultColor

            Text {
                anchors.centerIn: parent
                text: qsTr("User Preferences")
                color: userPreferencesSelected ? selectedTextColor : defaultTextColor
                font.pointSize: 12
            }

            MouseArea {
                anchors.fill: parent
                onClicked: selected = userPreferencesIndex
            }
        }
    }

    Rectangle {
        height: 2
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        color: Material.accentColor
    }
}
