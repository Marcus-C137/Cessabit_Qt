import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3
import QtQuick.VirtualKeyboard 2.1

Page {
    id: port
    visible: true
    property int alm_btn_pressed: 0;
    property int time_btn_pressed: 0;
    property string timeFormat: "h:mm:ss AP"
    background: Rectangle{
        anchors.fill: parent
        color: "black"
    }

    Component.onCompleted: {
        console.log("Port.qml loaded");
        chartDataObj.loadTemps("_min");
        cv_port.animationOptions = ChartView.SeriesAnimations
        chartDataObj
    }

    function toJSDate(qdate){
        console.log("qdate: " + qdate.getTime());
        var dateObj = new Date(qdate.getTime());
        console.log("dateObj " + dateObj)
        return dateObj;
    }

    Button {
        id: toolButton
        anchors.leftMargin: 20
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 20
        contentItem: Text {
            /// \u25C0 = back
            /// \u2630 = hamburger
            text: "\u25C0"
            color: "white"
            font.pointSize: 24
        }

        onClicked: {
            stack.pop()
        }

        background: Rectangle{
            anchors.fill: parent
            color: "black"
        }
    }

    Connections{
        target: chartDataObj
        onPortPowerUpdate: {
            txt_powerUsage.text =  " - Power: "+ (power * 100).toFixed(0) + "%";
        }

        onPortTempRefresh: {
            dataSeries.clear();
            var timeStamps = [];
            var min = 0;
            var max = 0;
            console.log("times.length = " + times.length)
            for (var i=0; i < times.length; i++){
                var jsDate = new Date(times[i].getTime());
                dataSeries.append(i, temps[i]); //console.log("onPortTempRefresh: dataSeries: at i= " + i + " " + dataSeries.at(i));
                timeStamps[i] = Qt.formatDateTime(jsDate, port.timeFormat); //console.log("onPortTempRefresh: timePrint: at i= " + i + " " + timePrint[i])
                if (temps[i] < min) min = temps[i]
                if (temps[i] > max) max = temps[i]
            }
            vAxisY.min = min - 10
            vAxisY.max = max + 10
            //console.log("timeStamps.length = " + timeStamps.length);
            bAxisX.categories = timeStamps; console.log("timeStamps:" +  timeStamps);
            //console.log("bAxisX.categories.length = " + bAxisX.categories.length);
        }
        onPortDataAdd:{
            //console.log("In port Data Add")
            //console.log("time = " + time);
            //console.log("temp = " + temp);
            var min = 0;
            var max = 0;
            var jsDate = new Date(time.getTime());
            var timePrint = Qt.formatDateTime(jsDate, port.timeFormat)
            var timeStamps = [];

            //Add to dataSeries
            for(var j = 0; j<dataSeries.count; j++){
                if (min > dataSeries.at(j).y) min = dataSeries.at(j).y
                if (max < dataSeries.at(j).y) max = dataSeries.at(j).y
                //console.log("Data series of " + j + " = " + dataSeries.at(j))
                if(j === dataSeries.count-1) {
                    dataSeries.replace(j,dataSeries.at(j).y, j, temp);
                }else{
                    dataSeries.replace(j,dataSeries.at(j).y, j, dataSeries.at(j+1).y)
                }
            }

            //Assign to xAxis
            for(var i = 0; i < bAxisX.categories.length; i++){
                timeStamps[i] = bAxisX.categories[i];
            }
            timeStamps.shift();
            timeStamps.push(timePrint);
            //console.log("timeStamps= " + timeStamps);

            vAxisY.min = min - 10
            vAxisY.max = max + 10
            bAxisX.categories = timeStamps
            //console.log("Data Series length = " + dataSeries.count)
        }
        onSetAlarmsRefresh: {
            btn_setTemp_txt.text = temps[0];
            btn_highTemp_txt.text = temps[1];
            btn_lowTemp_txt.text = temps[2];
        }
        onNewAlarmTempUpdate: {
            if (alarmLabel == 0){
                btn_setTemp_txt.text = Val;
            }else if(alarmLabel == 1){
                btn_highTemp_txt.text = Val;
            }else if(alarmLabel == 2){
                btn_lowTemp_txt.text = Val;
            }
        }
        onPortLabelRefresh: {
            txt_label.text = "Port " + port;
            switch(port){
            case 1:
                dataSeries.color = "blue";
                break;
            case 2:
                dataSeries.color = "green";
                break;
            case 3:
                dataSeries.color = "red";
                break;
            case 4:
                dataSeries.color = "yellow";
                break;
            }
        }
    }

        Text{
            id: txt_label
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 18
            text: ""
            color: "white"
            font.pointSize: 28

        }

        Text{
            id: txt_powerUsage
            anchors.left: txt_label.right
            anchors.bottom: txt_label.bottom
            anchors.topMargin: 18
            text: ""
            color: "white"
            font.pointSize: 28
        }

        //////////////////Chart////////////////////////

        ChartView {
            id: cv_port
            x: 170
            y: 69
            width: 300
            height: 300
            anchors.left: parent.left
            anchors.right: parent.right
            legend.visible: false
            animationOptions: ChartView.NoAnimation
            backgroundColor: "black"
            antialiasing: true

            ValueAxis {
                id: vAxisY
                min: 70
                max: 100
                gridVisible: false
                color: "#ffffff"
                labelsColor: "#ffffff"
                labelFormat: "%.0f"
            }

            BarCategoryAxis {
                id: bAxisX
                gridVisible: false
                color: "#ffffff"
                labelsColor: "#ffffff"

            }

            LineSeries {
                id: dataSeries
                name: "LineSeries"
                width: 4.0
                color: "white"
                axisX: bAxisX
                axisY: vAxisY
                capStyle: Qt.RoundCap;
                pointsVisible: true;
                pointLabelsColor: "green"
            }
        }

        Item{
            id: i_chartSelector
            anchors.top: cv_port.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: -20
            height: 30
            width: 165
            Rectangle{
                anchors.fill: parent
                radius: 15
                color: "grey"
            }
            Button{
                id: btn_chartSelector_min
                anchors.left: parent.left
                height: 30
                width: 30
                text: "m"
                background: Rectangle{
                    id: btn_txt_chartSelector_min
                    anchors.fill: parent
                    radius: 15
                    color: "white"
                }
                onClicked:{
                    port.timeFormat="h:mm:ss AP"
                    chartDataObj.loadTemps("_min");
                    btn_txt_chartSelector_min.color ="white"
                    btn_txt_chartSelector_hour.color ="dark grey"
                    btn_txt_chartSelector_day.color ="dark grey"
                    btn_txt_chartSelector_month.color ="dark grey"
                }
            }

            Button{
                id: btn_chartSelector_hour
                anchors.left: btn_chartSelector_min.right
                anchors.leftMargin: 15
                height: 30
                width: 30
                text: "h"
                background: Rectangle{
                    id: btn_txt_chartSelector_hour
                    anchors.fill: parent
                    radius: 15
                    color: "dark grey"
                }
                onClicked:{
                    port.timeFormat="hh:mm AP";
                    chartDataObj.loadTemps("_hour");
                    btn_txt_chartSelector_min.color ="dark grey"
                    btn_txt_chartSelector_hour.color ="white"
                    btn_txt_chartSelector_day.color ="dark grey"
                    btn_txt_chartSelector_month.color ="dark grey"
                }
            }

            Button{
                id: btn_chartSelector_day
                anchors.left: btn_chartSelector_hour.right
                anchors.leftMargin: 15
                height: 30
                width: 30
                text: "d"
                background: Rectangle{
                    id: btn_txt_chartSelector_day
                    anchors.fill: parent
                    radius: 15
                    color: "dark grey"
                }
                onClicked:{
                    port.timeFormat = "h AP"
                    chartDataObj.loadTemps("_day");
                    btn_txt_chartSelector_min.color ="dark grey"
                    btn_txt_chartSelector_hour.color ="dark grey"
                    btn_txt_chartSelector_day.color ="white"
                    btn_txt_chartSelector_month.color ="dark grey"
                }
            }
            Button{
                id: btn_chartSelector_month
                anchors.left: btn_chartSelector_day.right
                anchors.leftMargin: 15
                height: 30
                width: 30
                text: "m"
                background: Rectangle{
                    id: btn_txt_chartSelector_month
                    anchors.fill: parent
                    radius: 15
                    color: "dark grey"
                }
                onClicked:{
                    port.timeFormat = "ddd dAP"
                    chartDataObj.loadTemps("_month");
                    btn_txt_chartSelector_min.color ="dark grey"
                    btn_txt_chartSelector_hour.color ="dark grey"
                    btn_txt_chartSelector_day.color ="dark grey"
                    btn_txt_chartSelector_month.color ="white"
                }
            }

        }

        //////////////////// Set Alarms /////////////////////////

        Item{
            id: setTempBox
            height:50
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: i_chartSelector.bottom
            anchors.leftMargin: 100
            anchors.rightMargin: 100
            Text{
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                font.pointSize: 18
                text: "Set Point"
                color: "white"
            }
            Button{
                id: btn_setTemp
                width: 100
                anchors.right: parent.right
                contentItem: Text {
                    id: btn_setTemp_txt
                    anchors.fill: parent
                    text: ""
                    color: "white"
                    font.pointSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle{
                    anchors.fill: parent
                    color: "black"
                    border.color: "teal"
                    radius: 5
                }
                onClicked: {
                    port.alm_btn_pressed = 1;
                    popup_enterNewTemp.open();
                }
            }
            Rectangle{
                color: "white"
                height: 2
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.bottom: parent.bottom
            }
        }

        Item{
            id: highTempBox
            height:50
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: setTempBox.bottom
            anchors.leftMargin: 100
            anchors.rightMargin: 100
            anchors.topMargin: 10
            Text{
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                font.pointSize: 18
                text: "High Alarm"
                color: "white"
            }
            Button{
                id: btn_highTemp
                width: 100;
                anchors.right: parent.right
                contentItem: Text {
                    anchors.fill: parent
                    id: btn_highTemp_txt
                    text: ""
                    color: "white"
                    font.pointSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle{

                    anchors.fill: parent
                    color: "black"
                    border.color: "teal"
                    border.width: 2
                    radius: 5
                }
                onClicked: {
                    port.alm_btn_pressed = 2;
                    popup_enterNewTemp.open();
                }

            }
            Rectangle{
                color: "white"
                height: 2
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.bottom: parent.bottom
            }
        }

        Item{
            id: lowTempBox
            height:50
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: highTempBox.bottom
            anchors.leftMargin: 100
            anchors.rightMargin: 100
            anchors.topMargin: 10
            Text{
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                font.pointSize: 18
                text: "Low Alarm"
                color: "white"
            }
            Button{
                id: btn_lowTemp
                anchors.right: parent.right
                width: 100;
                contentItem: Text {
                    anchors.fill: parent
                    id: btn_lowTemp_txt
                    text: ""
                    color: "white"
                    font.pointSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle{
                    width: 50;
                    anchors.fill: parent
                    color: "black"
                    border.color: "teal"
                    border.width: 2
                    radius: 5
                }
                onClicked: {
                    port.alm_btn_pressed = 3;
                    popup_enterNewTemp.open();
                }
            }
            Rectangle{
                color: "white"
                height: 2
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.bottom: parent.bottom
            }
        }

        /////////////////////POPUP////////////////////////////

        Popup{
            id:popup_enterNewTemp
            x:(parent.width-popup_enterNewTemp.width)/2
            y:75
            closePolicy: Popup.NoAutoClose
            width: 600
            height: 250
            modal: false
            focus: true
            background: Rectangle{
                anchors.fill: parent
                color: "black"
                border.color: "#56a0d4"
            }
            onOpened:{
                if (alm_btn_pressed === 1){
                    txt_oldTemp.text = "Current Set Point: " + btn_setTemp_txt.text + " F"
                    txt_newTemp.text = "Enter new Set Point"
                }
                if (alm_btn_pressed === 2){
                    txt_oldTemp.text = "Current High Alarm: " + btn_highTemp_txt.text + " F"
                    txt_newTemp.text = "Enter new High Alarm"
                }
                if (alm_btn_pressed === 3){
                    txt_oldTemp.text = "Current Low Alarm: " + btn_lowTemp_txt.text + " F"
                    txt_newTemp.text = "Enter new Low Alarm"
                }
            }
            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 20
                id:txt_oldTemp
                text: ""
                color: "white"
                font.pixelSize: 26
            }
            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: txt_oldTemp.bottom
                anchors.topMargin: 20
                id:txt_newTemp
                text:""
                color: "white"
                font.pixelSize: 26
            }
            Item{
                height: 40
                width: 120
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: txt_newTemp.bottom
                anchors.topMargin: 12
                Rectangle{
                    color: "white"
                    anchors.fill: parent
                }
                TextInput{
                    id:txtI_new
                    anchors.fill: parent
                    inputMethodHints: Qt.ImhDigitsOnly;
                    color: "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 26
                    maximumLength: 5;
                }

            }

            Button{
                anchors.right: parent.right;
                anchors.bottom: parent.bottom;
                anchors.rightMargin: 25
                contentItem: Text {
                    text: "Enter"
                    color: "white"
                    font.pointSize: 18

                }
                background: Rectangle{
                    anchors.fill: parent
                    color: "red"
                    radius: 5
                }
                onClicked: {
                    chartDataObj.storeNewAlarmTemp(alm_btn_pressed-1,txtI_new.text)
                    popup_enterNewTemp.close();
                }
            }
            Button{
                anchors.left: parent.left;
                anchors.bottom: parent.bottom;
                anchors.leftMargin: 25
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
                onClicked: {
                    popup_enterNewTemp.close();
                }
            }

        }

    }

    /*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
