import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.VirtualKeyboard 2.1


Page{
    id: account
    visible: true
    background: Rectangle{
        anchors.fill: parent
        color: "black"
    }
    Component.onCompleted: {
        console.log("Account.qml loaded");
    }
    Connections{
        target: firebase;
        onSignInResults:{
            popUpSignedInPage.visible=true
            busyIndi.visible = true
            popUpStartPage.visible = false
            if (successful){
                txt_signedInResults.text = "Account link successfull! You have now linked your account"
                txt_signedInInfo.text = firebase.signedIn ? "Signed in as: " + firebase.uname : "You're currently not signed in.\nDownload the app and make an account.\nOnce done, enter in your info to link your device"
            }else{
                txt_signedInResults.text = "Error linking account. Username and password are incorrect"
            }
        }
        onSubResults:{
            busyIndi_sub.visible = false
            if (subscribed){
                txt_subResults.text = "You are subscribed!"
            }else{
                txt_subResults.text = "Currently not subscribed. Subscribe through the app under subscriptions"
            }
        }
    }

    /////////////////NOT SIGNED IN/////////////////////

    Text{
        id: txt_signedInInfo
        y:200
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        text: firebase.signedIn ? "Signed in as: " + firebase.uname : "You're currently not signed in.\nDownload the app and make an account.\nOnce done, enter in your info to link your device"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 26

    }

    Button {
        id: btn_SignIn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: txt_signedInInfo.bottom
        anchors.topMargin: 40
        contentItem: Text {
            text: firebase.signedIn ? "Link To A Different Account" : "Connect to an Account"
            color: "white"
            font.pointSize: 24
        }

        onClicked: {
            popup_enterAccountInfo.open()
        }

        background: Rectangle{
            anchors.fill: parent
            color: Qt.rgba(0,186/255, 169/255, 1);
            radius: 30
        }

    }

    Button{
        id:btn_ChkSub
        visible: firebase.signedIn ? true : false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        contentItem: Text{
            text: "check Subscription"
            color: "white"
            font.pointSize: 24
        }
        onClicked: {
            firebase.checkSub()
            popUp_sub.open()
        }
        background: Rectangle{
            anchors.fill: parent
            color: Qt.rgba(0,186/255, 169/255, 1);
            radius: 30
        }
    }

    /////////////////////POPUPS////////////////////////////
    Popup{
        id:popUp_sub
        x:(parent.width-popup_enterAccountInfo.width)/2
        y:75
        closePolicy: Popup.NoAutoClose
        width: 800
        height: 300
        modal: false
        focus: true
        background: Rectangle{
            anchors.fill: parent
            color: "black"
            border.color: "#56a0d4"
        }
        onOpened:{
            popUpSignedInPage.visible = false
            popUp_sub.visible = true
            busyIndi.visible = true
        }
        BusyIndicator{
            id: busyIndi_sub
            running: true
            anchors.centerIn: parent
            width: 150
            height: 150
        }
        Text{
            id: txt_subResults
            anchors.centerIn: parent
            text: "";
            color: "white"
            font.pointSize: 18
        }
        Button{
            id: btn_popUpSub
            width: 100
            anchors.top: txt_subResults.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 50
            anchors.leftMargin: 200
            anchors.rightMargin: 200
            contentItem: Text {
                text: "Ok"
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 18
            }
            background: Rectangle{
                anchors.fill: parent
                color: "green"
                radius: 5
            }
            onClicked: {
                popUp_sub.close();
            }
        }
    }

    Popup{
        id:popup_enterAccountInfo
        x:(parent.width-popup_enterAccountInfo.width)/2
        y:75
        closePolicy: Popup.NoAutoClose
        width: 800
        height: 300
        modal: false
        focus: true
        background: Rectangle{
            anchors.fill: parent
            color: "black"
            border.color: "#56a0d4"
        }
        onOpened:{
            popUpSignedInPage.visible = false
            busyIndi.running = false
            popUpStartPage.visible = true
        }

        BusyIndicator{
            id: busyIndi
            running: false
            anchors.centerIn: parent
            width: 150
            height: 150
        }
        Item{
            id: popUpSignedInPage
            visible: false
            anchors.fill: parent
            Text{
                id: txt_signedInResults
                anchors.centerIn: parent
                text: "";
                color: "white"
                font.pointSize: 18
            }
            Button{
                id: btn_popUpSignInPage
                width: 100
                anchors.top: txt_signedInResults.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 50
                anchors.leftMargin: 200
                anchors.rightMargin: 200
                contentItem: Text {
                    text: "Ok"
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 18
                }
                background: Rectangle{
                    anchors.fill: parent
                    color: "green"
                    radius: 5
                }
                onClicked: {
                    popup_enterAccountInfo.close();
                }
            }
        }

        Item{
            id: popUpStartPage
            visible: false;
            anchors.fill: parent
            Text{
                id: txt_userName
                width: 150
                height: 40
                color: "white"
                font.pointSize: 20
                anchors.horizontalCenter: parent.Center
                anchors.top: parent.top
                anchors.topMargin: 50
                anchors.left : parent.left
                anchors.leftMargin: 40
                text: "Username"
            }
            TextInput{
                id: iTxt_username
                color: "white"
                width: 500
                height: 40
                font.pointSize: 20
                anchors.left: txt_userName.right
                anchors.leftMargin: 20
                anchors.bottom: txt_userName.bottom

            }

            Rectangle{
                color:"grey"
                width:  iTxt_username.width
                height: 2
                anchors.left: iTxt_username.left
                anchors.right: iTxt_username.right
                anchors.top: iTxt_username.bottom
            }
            Text{
                id: txt_password
                width: 150
                height: 40
                color: "white"
                anchors.horizontalCenter: parent.Center
                anchors.top: txt_userName.bottom
                anchors.topMargin: 30
                anchors.left : parent.left
                anchors.leftMargin: 40
                font.pointSize: 20
                text: "Password"
            }
            TextInput{
                id: iTxt_password
                color: "white"
                height: 40
                width: 500
                font.pointSize: 20
                anchors.left: txt_password.right
                anchors.leftMargin: 20
                anchors.bottom: txt_password.bottom

            }
            Rectangle{
                color: "grey"
                width:  iTxt_password.width
                height: 2
                anchors.left: iTxt_password.left
                anchors.right: iTxt_password.right
                anchors.top: iTxt_password.bottom
            }


            Button{
                id: btn_close
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 100
                onClicked: {
                    popup_enterAccountInfo.close();
                }
                contentItem: Text {
                    text: "Cancel"
                    color: "white"
                    font.pointSize: 18

                }
                background: Rectangle{
                    anchors.fill: parent
                    color: "red"
                    radius: 5
                }
            }

            Button{
                id: btn_ok
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 100
                onClicked: {
                    firebase.linkAccount(iTxt_username.text, iTxt_password.text);
                }
                contentItem: Text {
                    text: "Sign in"
                    color: "white"
                    font.pointSize: 18

                }
                background: Rectangle{
                    anchors.fill: parent
                    color: "red"
                    radius: 5
                }
            }
        }
    }


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



}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
