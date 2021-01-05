import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4

Flickable {

    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    contentWidth: parent.width
    contentHeight: 1000

    Component.onCompleted: {
        firebase.notifSetting_pushNotifications ? sw_pushNotifications.checked = true   : sw_pushNotifications.checked = false;
        firebase.notifSetting_textMe            ? sw_textMe.checked = true              : sw_textMe.checked = false;
        firebase.notifSetting_textFriends       ? sw_textFriends.checked = true          : sw_textFriends.checked = false;
    }
    Connections{
        target: firebase
        onNotifSetting_pushNotificationsChanged:{
            sw_pushNotifications.checked = notifSetting_pushNotifications;
        }
        onNotifSetting_textMeChanged:{
            sw_textMe.checked = notifSetting_textMe;
        }
        onNotifSetting_textFriendsChanged:{
            sw_textFriends.checked = notifSetting_textFriends;
        }
    }

    Rectangle{
        anchors.fill: parent
        color: "black"
        ///////////////////////BACK BUTTON //////////////////////
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

        ///////////////////////HEADER //////////////////////
        Label{
            id: lbl_header
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: toolButton.verticalCenter
            font.pointSize: 28
            text: "Settings"
            color: "white"
        }
        Rectangle{
            id: rec_header
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: toolButton.bottom
            anchors.topMargin: 10
            color: "white"
            height: 3
        }

        //////////////////////NOTIFICATIONS /////////////////


        Label{
            id:itm_notifications
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: itm_sensorDisconnect.bottom
            anchors.topMargin: 60
            text: "Notifications"
            color: "white"
            font.pixelSize: 38
        }
        Item{
            id: itm_pushNotifications
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_notifications.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "Push Notifications"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_pushNotifications
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_pushNotifications.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_pushNotifications.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_pushNotifications.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_pushNotifications.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_pushNotifications.down ? "#cccccc" : "#ffffff"
                        border.color: sw_pushNotifications.checked ? (sw_pushNotifications.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(6, sw_pushNotifications.checked)
                }
            }
            Rectangle{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                color: "teal"
            }
        }

        Item{
            id: itm_textMe
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_pushNotifications.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "Text me"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_textMe
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_textMe.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_textMe.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_textMe.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_textMe.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_textMe.down ? "#cccccc" : "#ffffff"
                        border.color: sw_textMe.checked ? (sw_textMe.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(7, sw_textMe.checked)
                }
            }
            Rectangle{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                color: "teal"
            }
        }

        Item{
            id: itm_textFriends
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_textMe.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "Text Friends"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_textFriends
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_textFriends.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_textFriends.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_textFriends.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_textFriends.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_textFriends.down ? "#cccccc" : "#ffffff"
                        border.color: sw_textFriends.checked ? (sw_textFriends.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(8, sw_textFriends.checked)
                }
            }
            Rectangle{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                color: "teal"
            }
        }


    }


}

