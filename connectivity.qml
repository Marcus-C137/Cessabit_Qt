import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.VirtualKeyboard 2.1

Page {
    id: connectivity
    visible: true
    background: Rectangle{
        color: "black"
        anchors.fill: parent
    }

    //////////LOGIC//////////////

    Component.onCompleted: {
        console.log("Connectivity.qml loaded");
        wifiManager.checkWifi()
    }

    Connections{
        target: wifiManager
        onNewWifiRSSI:{
            console.log("in onNewWifiRSSI: " + connected);
            rec_newWifiLoading.visible = false;
            if(connected){
                wifiStatus.visible            = true;
                progressBar.visible           = true;
                progressBarBackGround.visible = true;
                tickPoor.visible              = true;
                tickAverage.visible           = true;
                tickExcellent.visible         = true;
                txt_tickPoor.visible          = true;
                txt_tickAverage.visible       = true;
                txt_tickExcellent.visible     = true;
                txt_sigStrength.visible       = true;
                progressBar.value             = val;
                wifiStatus.text               = "Connected to " + SSID;
                animationLoadSignal.start()
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
            popup_ListView.model = SSIDs;
            busyIndi.running = false;
            popup_ScrollView.visible = true;

        }
    }

    ///////////////POPUPS//////////////////

    Popup{
        id: popup_scannedNetworks
        anchors.centerIn: parent
        width: 500
        height: 500
        //closePolicy: Popup.CloseOnPressOutside
        modal: false
        focus: true
        background: Rectangle{
            anchors.fill: parent
            color: "black"
        }

        onOpened: {
            wifiManager.scanSSIDs();
            busyIndi.running = true
            popup_ScrollView.visible = false;
        }
        BusyIndicator{
            id: busyIndi
            running: true
            anchors.centerIn: parent
            width: 150
            height: 150
        }

        ScrollView{
            id:popup_ScrollView
            anchors.fill: parent
            visible: false
            ListView{
                id:popup_ListView
                width:parent.width
                clip: true
                model: 0
                delegate: ItemDelegate{
                    id: popup_itemDelegate
                    height: 90
                    //text: modelData
                    Text{
                        id:txt_SSIDname
                        color: "white"
                        text: modelData
                        font.pixelSize: 28
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Rectangle{
                        height: 2
                        anchors.left: parent.left
                        anchors.right: parent.right
                        width: parent.width
                        anchors.top: txt_SSIDname.bottom
                        color: "#56a0d4"
                    }

                    width: parent.width
                    onClicked: {
                        popup_SSIDselected.open();
                        txt_SSIDselected.ssid = modelData;
                        txt_password.text = "";
                    }
                }
            }
        }

        Popup{
            id:popup_SSIDselected
            //x:popup_scannedNetworks.width - popup_SSIDselected.width/2
            x:(popup_scannedNetworks.width-popup_SSIDselected.width)/2
            closePolicy: Popup.NoAutoClose
            width: 400
            height: 200
            modal: false
            focus: true
            background: Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "#56a0d4"
            }
            Text{
                id: txt_SSIDselected
                width: 400
                height: 50
                color: "white"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 10
                font.pointSize: 16
                property var ssid: ""
                text: "Password for " + ssid
            }
            TextInput{
                id: txt_password
                width: 400
                height: 50
                anchors.top: txt_SSIDselected.bottom
                color: "white"
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
                    wifiManager.resetTimer();
                    progressBar.value = 0;
                    rec_newWifiLoading.visible = true
                    animationLoadForward.start()
                    popup_SSIDselected.close()
                    popup_scannedNetworks.close()
               }
            }
        }


    }

    //////////////////LOADER//////////////////////////

    Rectangle{
        id: rec_newWifiLoading
        height: 20
        width: 0
        x: 260
        y: 300
        radius: 20
        color: "red"
        visible: false;
        PropertyAnimation{
            id:animationLoadForward
            target: rec_newWifiLoading
            property: "width"
            to: 500
            duration: 1000
            easing.type: Easing.InOutQuad
            onFinished: {
                animationLoadBack.start()
            }

        }
        PropertyAnimation{
            id:animationLoadBack
            target: rec_newWifiLoading
            property: "width"
            to: 0
            duration: 1000
            easing.type: Easing.InOutQuad
            onFinished: {
                animationLoadForward.start()
            }
        }
    }

    /////////////BACK BUTTON//////////////////

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

    /////////////////////MAIN UI//////////////////

    Text {
        id: wifiStatus
        width: 300
        height: 50
        y:100
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 28
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
            width: 0
            height: parent.height
            property var value: 0.5
            gradient: Gradient{
                GradientStop{position: 0.0; color: Qt.rgba(0.9, 0.1, 0, 1)}
                GradientStop{position: 1.0; color: Qt.rgba(1-progressBar.width, progressBar.width, 0, 1)}
                orientation: Gradient.Horizontal;
            }
            PropertyAnimation{
                id:animationLoadSignal
                target: progressBar
                property: "width"
                to: progressBarBackGround.width * progressBar.value
                duration: 3000
                easing.type: Easing.InOutQuad
            }
        }
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



/*##^##
Designer {
    D{i:0;formeditorZoom:0.8999999761581421;height:600;width:1024}
}
##^##*/
