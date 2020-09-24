import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480

    StackView {

        id: stack
        initialItem: "Home.qml"
        anchors.fill: parent

    }


}
