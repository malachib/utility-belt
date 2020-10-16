import QtQuick 2.11
import QtQuick.Window 2.11

import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    TabBar {
        id: bar
        width: parent.width

        Repeater {
            model: PluginServicesModel

            TabButton {
                text: modelData.name + " (" + modelData.version + ")"
            }
        }
    }

    StackLayout {
        currentIndex: bar.currentIndex
        anchors.top: bar.bottom

        Repeater {
            model: PluginServicesModel

            Loader {
                sourceComponent: modelData.component
            }

            Text {
                text: modelData.status2
            }
        }
    }
}
