import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import com.svarmed.backend 1.0

RowLayout {
    id: root

    readonly property Device device: facade.device

    enabled: device.availablePorts.length > 0
    spacing: 10

    Item {
        Layout.fillWidth: true
    }

    SpinBox {
        id: baudRateSpinBox

        editable: true
        from: device.bottomBaudRate
        to: device.topBaudRate
        enabled: !device.opened && !device.busy
        value: device.baudRate

        Binding {
            target: device
            property: "baudRate"
            value: baudRateSpinBox.value
        }
    }

    ComboBox {
        id: portComboBox

        model: device.availablePorts
        enabled: !device.opened && !device.busy

        Binding {
            target: device
            property: "selectedPort"
            value: portComboBox.currentIndex
        }

        Binding {
            target: portComboBox
            property: "currentIndex"
            value: device.selectedPort
        }
    }

    Button {
        text: device.opened ? qsTr("Disconnect") : qsTr("Connect")

        onClicked: {
            if (device.opened) {
                device.close()
            }
            else {
                device.open()
            }
        }
    }
}
