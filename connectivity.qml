import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    id: connectivity
    visible: true
    width: 640
    height: 480

    Component.onCompleted: {
        console.log("Connectivity.qml loaded");
    }

    Button {
        id: toolButton

        contentItem: Text {
            /// \u25C0 = back
            /// \u2630 = hamburger
            text: "\u25C0"
            color: "white"
            font.pointSize: 32

        }

        onClicked: {
            drawer.open()
        }

        background: Rectangle{
            anchors.fill: parent
            color: "black"
        }
        anchors.leftMargin: 20
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 20


    }

}
