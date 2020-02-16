import QtQuick 2.0

Item {
    id: root

    readonly property var settings: device.settings


    ListModel {
        id: settingsModel

        ListElement {
            label: "Serialized data page"
            minValue: settings.minSerializedDataPage
            maxValue: settings.maxSerializedDataPage
            valueName: "serializedDataPage"
        }

        ListElement {
            label: "Pwm clock"
            minValue: settings.minPwmClock
            maxValue: settings.maxPwmClock
            valueName: "pwmClock"
        }

        ListElement {
            label: "Serialized data page"
            minValue: settings.minSerializedDataPage
            maxValue: settings.maxSerializedDataPage
            valueName: "serializedDataPage"
        }

        ListElement {
            label: "Serialized data page"
            minValue: settings.minSerializedDataPage
            maxValue: settings.maxSerializedDataPage
            valueName: "serializedDataPage"
        }
    }

    ListView {
        anchors.fill: parent

    }
}
