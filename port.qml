import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

Page {
    id: port
    Component.onCompleted: {
        chartDataObj.port1BtnPressed();
    }

    Rectangle {
        id: rec_port1
        anchors.left: parent.left
        anchors.top: parent.top
        width: 50
        height: 50
        color: "black"
        Text{
            text: qsTr("96.5")
            color: "white"
            font.pointSize: 32
            anchors.centerIn: parent
        }
        TapHandler{
            id: tap_port1
            onTapped: {
                console.log("tapped");
                homeStackView.replace("Home.qml");
            }
        }
    }

    Connections{
        target: chartDataObj
        onPortRefreshData: {
            dataSeries.clear();
            var timePrint = [];
            console.log("IN ON PORT REFRESH RATE")
            console.log("times.length = " + times.length)
            console.log("time is of type " +  typeof times)
            for (var i=0; i < times.length; i++){
                console.log("i = " + i);
                dataSeries.append(i, temps[i])
                var jsDate = new Date(times[i].getTime());
                var hour = jsDate.getHours();
                var minute = jsDate.getMinutes();
                var seconds = jsDate.getSeconds();
                var hrMn = hour + ":" + minute + ":" + seconds;
                timePrint[i] = hrMn;
                console.log("DataSeries: " + dataSeries.at(0))

            }

            bAxisX.categories = timePrint
            //line.createSeries(ChartView.SeriesTypeLine, "nothing", dtAxisX, vAxisY);
        }
        onPortDataAdd:{
            console.log("In port Data Add")
            console.log("time = " + time);
            console.log("temp = " + temp);
            var jsDate = new Date(time.getTime());
            var hour = jsDate.getHours();
            var minute = jsDate.getMinutes();
            var seconds = jsDate.getSeconds();
            var hrMn = hour + ":" + minute + ":" + seconds;
            dataSeries.append(dataSeries.count, temp)
            var categories = [];
            for(var i = 0; i < bAxisX.categories.length - 1 ; i++){
                categories[i] = bAxisX.categories[i+1];
            }
            categories.push(hrMn);

            bAxisX.categories = categories

        }
    }

    ChartView {
        id: line
        x: 170
        y: 69
        width: 300
        height: 300
        anchors.left: parent.left
        anchors.right: parent.right
        legend.visible: false
        //animationOptions: ChartView.SeriesAnimations
        backgroundColor: "black"
        antialiasing: true

        ValueAxis {
            id: vAxisY
            min: -200
            max: -190
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
            color: "white"
            axisX: bAxisX
            axisY: vAxisY
        }
    }

    function toJSDate(qdate){
        console.log("qdate: " + qdate.getTime());
        var dateObj = new Date(qdate.getTime());
        console.log("dateObj " + dateObj)
        return dateObj;
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
