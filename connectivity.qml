import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12


Page {
    id: connectivity
    visible: true
    background: Rectangle{
        color: "black"
        anchors.fill: parent
    }

    Component.onCompleted: {
        console.log("Connectivity.qml loaded");
        wifiManager.checkWifi()
    }

    Connections{
        target: wifiManager
        onNewWifiRSSI:{
            console.log("in onNewWifiRSSI");
            if(connected){
                wifiStatus.visible            = true;
                progressBar.visible           = true;
                tickPoor.visible              = true;
                tickAverage.visible           = true;
                tickExcellent.visible         = true;
                txt_tickPoor.visible          = true;
                txt_tickAverage.visible       = true;
                txt_tickExcellent.visible     = true;
                txt_sigStrength.visible       = true;
                progressBar.value             = val;
                wifiStatus.text               = "Connected to " + SSID;
            }else{
                wifiStatus.visible            = true;
                progressBarBackGround.visible = false;
                tickPoor.visible              = false;
                tickAverage.visible           = false;
                tickExcellent.visible         = false;
                txt_tickPoor.visible          = false;
                txt_tickAverage.visible       = false;
                txt_tickExcellent.visible     = false;
                txt_sigStrength.visible       = false;
                wifiStatus.text               = "Not Connected to Wifi";
            }

        }
        onNetworkStatusChanged:{
            if (connected){
                wifiStatus.text = "Currently connected to " + wifiSSID;
            }else{


            }
        }

        onNewScannedSSIDs:{
            console.log(SSIDs);
            popup_ListView.model = SSIDs

        }
    }

    Popup{
        id: popup_scannedNetworks
        anchors.centerIn: parent
        width: 500
        height: 500
        closePolicy: Popup.CloseOnPressOutside
        onOpened: {
            wifiManager.scanSSIDs();
        }
        Popup{
            id:popup_SSIDselected
            anchors.centerIn: parent
            width: 400
            height: 200
            Text{
                id: txt_SSIDselected
                width: 400
                height: 50
                anchors.top: parent.top
                font.pointSize: 16
                property var ssid: ""
                text: "Password for " + ssid
            }
            TextInput{
                id: txt_password
                anchors.top: txt_SSIDselected.bottom
                width: 400
                height: 50
                anchors.bottom: toolButton.top
                anchors.bottomMargin: -80
                anchors.left: toolButton.right
                anchors.leftMargin: -69
                anchors.right: toolButton.left
                anchors.rightMargin: -380
                anchors.horizontalCenter: toolButton.horizontalCenter
                anchors.verticalCenter: toolButton.verticalCenter
            }
            Button{
                id: btn_close
                text: "close"
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                onClicked: {
                    popup_SSIDselected.close();
                }
            }
            Button{
                id: btn_ok
                text: "Connect"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                onClicked: {
                    console.log("SSID: " + txt_SSIDselected.ssid + " password: " + txt_password.text);
                    wifiManager.connectToSSID(txt_SSIDselected.ssid, txt_password.text)
               }
            }
        }

        ScrollView{
            id:popup_ScrollView
            anchors.fill: parent
            ListView{
                id:popup_ListView
                width:parent.width
                clip: true
                model: 0
                delegate: ItemDelegate{
                    id: popup_itemDelegate
                    text: modelData
                    width: parent.width
                    onClicked: {
                        popup_SSIDselected.open();
                        txt_SSIDselected.ssid = modelData;
                    }
                }
            }
        }

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

    Rectangle {
        id: progressBarBackGround
        anchors.top: wifiStatus.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        width: 750
        height: 10
        color: "grey"
        Rectangle{
            id: progressBar
            anchors.left: parent.left
            anchors.top: parent.top
            width: parent.width * value
            height: parent.height
            property var value: 0.5
            gradient: Gradient{
                GradientStop{position: 0.0; color: Qt.rgba(0.9, 0.1, 0, 1)}
                GradientStop{position: 1.0; color: Qt.rgba(1-progressBar.value, progressBar.value, 0, 1)}
                orientation: Gradient.Horizontal;
            }
        }


    }


    Text {
        id: wifiStatus
        width: 300
        height: 50
        y:75
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 28
    }

    Rectangle {
        id: tickPoor
        anchors.top: progressBarBackGround.bottom
        anchors.topMargin: 5
        anchors.left: progressBarBackGround.left
        width: 3
        height: 19
        color: "#ffffff"
    }

    Rectangle {
        id: tickAverage
        anchors.top: progressBarBackGround.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: progressBarBackGround.horizontalCenter
        width: 3
        height: 19
        color: "#ffffff"
    }

    Rectangle {
        id: tickExcellent
        anchors.top: progressBarBackGround.bottom
        anchors.topMargin: 5
        anchors.right: progressBarBackGround.right
        width: 3
        height: 19
        color: "#ffffff"
    }

    Text {
        id: txt_tickPoor
        anchors.top: tickPoor.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: tickPoor.horizontalCenter
        width: 31
        height: 22
        color: "#ffffff"
        text: qsTr("Poor")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 16
    }

    Text {
        id: txt_tickAverage
        anchors.top: tickAverage.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: tickAverage.horizontalCenter
        width: 58
        height: 22
        color: "#ffffff"
        text: qsTr("Average")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 16
    }

    Text {
        id: txt_tickExcellent
        anchors.top: tickExcellent.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: tickExcellent.horizontalCenter
        width: 58
        height: 22
        color: "#ffffff"
        text: qsTr("Excellent")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 16
    }

    Text {
        id: txt_sigStrength
        anchors.top: txt_tickAverage.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: progressBarBackGround.horizontalCenter
        width: 314
        height: 22
        color: "#ffffff"
        text: qsTr("Signal Strength")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 22
    }

    Button {
        id: btn_changeWifi

        contentItem: Text {
            /// \u25C0 = back
            /// \u2630 = hamburger
            text: "Connect to a Network"
            color: "white"
            font.pointSize: 24

        }

        onClicked: {
            popup_scannedNetworks.open()
        }

        background: Rectangle{
            anchors.fill: parent
            color: Qt.rgba(0,186/255, 169/255, 1);
            radius: 30
        }
        anchors.bottomMargin: 40
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter


    }


}



/*##^##
Designer {
    D{i:0;formeditorZoom:0.8999999761581421;height:600;width:1024}
}
##^##*/
