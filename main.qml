import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtQuick.VirtualKeyboard 2.1

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    background: Rectangle{
        anchors.fill: parent
        color: "black"
    }

    StackView {

        id: stack
        initialItem: "Home.qml"
        anchors.fill: parent

    }

    /////////////////////KEYBORD////////////////////
    InputPanel {
        id: inputPanel
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
        anchors.left: parent.left
        anchors.right: parent.right
        z: 1
        parent: Overlay.overlay
        focus: true
    }


}
