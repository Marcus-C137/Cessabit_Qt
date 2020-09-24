import QtQuick 2.12
import QtQuick.Controls 2.5

Page{
    id: account
    visible: true

    Component.onCompleted: {
        console.log("Account.qml loaded");
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
            stack.pop()
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

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
