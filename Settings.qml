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
        firebase.almSetting_deviceOffline       ? sw_deviceOffline.checked = true       : sw_deviceOffline.checked = false;
        firebase.almSetting_lowTemp             ? sw_lowTemp.checked = true             : sw_lowTemp.checked = false;
        firebase.almSetting_highTemp            ? sw_highTemp.checked = true            : sw_highTemp.checked = false;
        firebase.almSetting_notResponsive       ? sw_notResponsive.checked = true       : sw_notResponsive.checked = false;
        firebase.almSetting_sensorDisconnect    ? sw_sensorDisconnect.checked = true    : sw_sensorDisconnect.checked = false;
        firebase.notifSetting_pushNotifications ? sw_pushNotifications.checked = true   : sw_pushNotifications.checked = false;
        firebase.notifSetting_textMe            ? sw_textMe.checked = true              : sw_textMe.checked = false;
        firebase.notifSetting_textFriends       ? sw_textFriends.checked = true          : sw_textFriends.checked = false;
    }
    Connections{
        target: firebase
        onAlmSetting_deviceOfflineChanged:{
            sw_deviceOffline.checked = almSetting_deviceOffline;
        }

        onAlmSetting_lowTempChanged:{
            sw_lowTemp.checked = almSetting_lowTemp;
        }
        onAlmSetting_highTempChanged:{
            sw_highTemp.checked = almSetting_highTemp;
        }
        onAlmSetting_notResponsiveChanged:{
            sw_notResponsive.checked = almSetting_notResponsive;
        }
        onAlmSetting_sensorDisconnectChanged:{
            sw_sensorDisconnect.checked = almSetting_sensorDisconnect;
        }
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

        //////////////////////ALARMS /////////////////
        Item{
            id:itm_alarmHeader
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: rec_header.bottom
            anchors.topMargin: 60
            Label{
                anchors.centerIn: parent
                text: "Alarms"
                color: "white"
                font.pixelSize: 38
            }
        }

        Item{
            id: itm_deviceOffline
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: itm_alarmHeader.bottom
            anchors.topMargin: 40
            height: 50
            Label {
                anchors.left: parent.left
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Device Offline"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_deviceOffline
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_deviceOffline.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_deviceOffline.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_deviceOffline.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_deviceOffline.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_deviceOffline.down ? "#cccccc" : "#ffffff"
                        border.color: sw_deviceOffline.checked ? (sw_deviceOffline.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(1, sw_deviceOffline.checked)
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
            id: itm_lowTemp
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: itm_deviceOffline.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Low Temperature"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_lowTemp
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_lowTemp.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_lowTemp.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_lowTemp.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_lowTemp.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_lowTemp.down ? "#cccccc" : "#ffffff"
                        border.color: sw_lowTemp.checked ? (sw_lowTemp.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(2, sw_lowTemp.checked)
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
            id: itm_highTemp
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_lowTemp.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "High Temperature"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_highTemp
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_highTemp.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_highTemp.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_highTemp.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_highTemp.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_highTemp.down ? "#cccccc" : "#ffffff"
                        border.color: sw_highTemp.checked ? (sw_highTemp.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(3, sw_highTemp.checked);
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
            id: itm_notResponsive
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_highTemp.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "Temperature Not Changing"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_notResponsive
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_notResponsive.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_notResponsive.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_notResponsive.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_notResponsive.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_notResponsive.down ? "#cccccc" : "#ffffff"
                        border.color: sw_notResponsive.checked ? (sw_notResponsive.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(4, sw_notResponsive.checked)
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
            id: itm_sensorDisconnect
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top:itm_notResponsive.bottom
            anchors.topMargin: 20
            height: 50
            Label {
                anchors.left: parent.left
                text: "Sensor Disconnect"
                color: "white"
                font.pixelSize: 28
            }
            Switch{
                anchors.right: parent.right
                id: sw_sensorDisconnect
                indicator: Rectangle {
                    implicitWidth: 70
                    implicitHeight: 22
                    x: sw_sensorDisconnect.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: 11
                    color: sw_sensorDisconnect.checked ? "lightgreen" : "#FF6969"
                    border.color: sw_sensorDisconnect.checked ? "lightgreen" : "#FF6969"
                    Rectangle {
                        x: sw_sensorDisconnect.checked ? parent.width - width : 0
                        anchors.verticalCenter: parent.verticalCenter
                        width: 38
                        height: 38
                        radius: 19
                        color: sw_sensorDisconnect.down ? "#cccccc" : "#ffffff"
                        border.color: sw_sensorDisconnect.checked ? (sw_sensorDisconnect.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
                onClicked: {
                    firebase.updateSettings(5, sw_sensorDisconnect.checked)
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

