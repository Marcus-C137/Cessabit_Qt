import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4

Page {

    Connections{
        target: arduinoComms
        onTempUpdate:{
            console.log("In on Temp update")
            port1_txt.text = temps[0].toFixed(1);
            port2_txt.text = temps[1].toFixed(1);
            port3_txt.text = temps[2].toFixed(1);
            port4_txt.text = temps[3].toFixed(1);
        }
    }

    ///////HEADER/////////////
    Rectangle{
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 70
        color: "black"

        Button {
            id: toolButton

            contentItem: Text {
                /// \u25C0 = back
                /// \u25C0 = hamburger
                text: "\u2630"
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


        }
        Text{
            id: timeText
            font.pointSize: 22
            color: "white"
            text: Qt.formatTime(new Date(), 'hh:mm AP')
            anchors.left: lbl_Cessabit.right
            leftPadding: 80
            anchors.bottom: parent.bottom
        }
        Text {
            id: lbl_Cessabit
            color: "#ffffff"
            text: "Cessabit"
            font.pointSize: 28
            anchors.centerIn: parent
        }

    }


    ////////////////DRAWER/////////////

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height
        background: Rectangle{
            color: "black"
        }

        Column {
            anchors.fill: parent

            ItemDelegate {
                id: connectivity_del
                contentItem: Text{
                    text: "Connectivity"
                    color: "white"
                    font.pixelSize: 28
                    padding: 20
                }

                width: parent.width
                onClicked: {
                    drawer.close()
                    stack.push("connectivity.qml")
                }

            }

            ItemDelegate {
                contentItem: Text{
                    text: "Account"
                    color: "white"
                    font.pixelSize: 28
                    padding: 20
                }

                width: parent.width
                onClicked: {
                    drawer.close()
                    stack.push("account.qml")
                }
            }

            ItemDelegate {
                contentItem: Text{
                    text: "Settings"
                    color: "white"
                    font.pixelSize: 28
                    padding: 20
                }
                width: parent.width
                onClicked: {
                    drawer.close()
                    stack.push("Settings.qml")
                }
            }
        }
    }

    Timer {
        id: timer
        interval: 1000
        repeat: true
        running: true

        onTriggered:
        {
            timeText.text = Qt.formatTime(new Date(), 'hh:mm AP');
        }
    }

    ///////// BUTTONS /////////////////////////

    Rectangle {
        id: rec_port1
        anchors.left: parent.left
        anchors.top: header.bottom
        width: parent.width/2
        height: (parent.height - header.height)/2
        color: "black"
        Text{
            id: port1_txt
            text: "Load..."
            color: "white"
            font.pointSize: 32
            anchors.centerIn: parent
        }
        TapHandler{
            id: tap_port1
            onTapped: {
                stack.push("port.qml")
            }
        }
    }

    Rectangle {
        id: rec_port2
        anchors.right: parent.right
        anchors.top: header.bottom
        width: parent.width/2
        height: (parent.height - header.height)/2
        color: "black"
        Text{
            id: port2_txt
            text: "Load..."
            color: "white"
            font.pointSize: 32
            anchors.centerIn: parent

        }
        TapHandler{
            id: tap_port2
            onTapped: {
                stack.push("port.qml")
            }
        }
    }

    Rectangle {
        id: rec_port3
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.width/2
        height: (parent.height - header.height)/2
        color: "black"
        Text{
            id: port3_txt
            text: "Load..."
            color: "white"
            font.pointSize: 32
            anchors.centerIn: parent
        }
        TapHandler{
            id: tap_port3
            onTapped: {
                stack.push("port.qml")
            }
        }
    }
    Rectangle {
        id: rec_port4
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width/2
        height: (parent.height - header.height)/2
        color: "black"
        Text{
            id: port4_txt
            text: "Load..."
            color: "white"
            font.pointSize: 32
            anchors.centerIn: parent
        }
        TapHandler{
            id: tap_port4
            onTapped: {
                stack.push("port.qml")
            }
        }
    }

    /////////////HEADER BORDER ///////////////
    Rectangle{
        anchors.left: header.left
        anchors.right: header.right
        anchors.top: header.bottom
        height: 3
        color: "white"
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
