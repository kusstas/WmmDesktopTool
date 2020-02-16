import QtQuick 2.14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Universal 2.12

ApplicationWindow {
    visible: true

    Universal.theme: Universal.Light
    Universal.accent: Universal.Cobalt

    width: 640
    height: 480

    ColumnLayout {
        anchors.fill: parent

        Tabs {
            id: tabs

            Layout.fillWidth: true
            Layout.preferredHeight: 35
        }

        Loader {
            Layout.fillHeight: true
            Layout.fillWidth: true

            readonly property var contentUrls: ["qrc:/Settings.qml", "qrc:/UserPreferences.qml"]

            source: contentUrls[tabs.selected]
        }

        Footer {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
        }
    }
}
