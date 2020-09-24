import QtQuick 2.12
import QtQuick.Controls 2.5
import QtCharts 2.3

Page {
    id: port
    visible: true
    background: Rectangle{
        anchors.fill: parent
        color: "black"
    }

    Component.onCompleted: {
        console.log("Port.qml loaded");
        chartDataObj.port1BtnPressed();
    }

    Button {
        id: toolButton

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
        anchors.leftMargin: 20
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 20


    }



    Connections{
        target: chartDataObj
        onPortRefreshData: {
            dataSeries.clear();
            var timePrint = [];
            var min = 90;
            var max = 80;
            console.log("IN ON PORT REFRESH RATE")
            console.log("times.length = " + times.length)

            for (var i=0; i < times.length; i++){
                console.log("i = " + i);
                var jsDate = new Date(times[i].getTime());
                timePrint[i] = Qt.formatTime(jsDate, 'hh:mm:ss AP')
                dataSeries.append(i, temps[i])
                console.log("DataSeries: " + dataSeries.at(i))
                if (temps[i] < min) min = temps[i]
                if (temps[i] > max) max = temps[i]
            }

            vAxisY.min = min - 10
            vAxisY.max = max + 10

            bAxisX.categories = timePrint
        }
        onPortDataAdd:{
            //console.log("In port Data Add")
            //console.log("time = " + time);
            //console.log("temp = " + temp);

            var min = 90;
            var max = 80;
            for(var j = 0; j<dataSeries.count; j++){
                if (min < dataSeries.at(j).y) min = dataSeries.at(j).y
                if (max > dataSeries.at(j).y) max = dataSeries.at(j).y
                console.log("Data series of " + j + " = " + dataSeries.at(j))
                if(j !== dataSeries.count-1) dataSeries.at(j).y = dataSeries.at(j+1).y
            }
            dataSeries.at(dataSeries.count-1).y = temp;

            var jsDate = new Date(time.getTime());
            var timePrint = Qt.formatTime(jsDate, 'hh:mm:ss AP')
            var categories = [];
            for(var i = 0; i < bAxisX.categories.length; i++){
                categories[i] = bAxisX.categories[i];
            }
            categories.shift();
            categories.push(timePrint);
            bAxisX.categories = categories
            console.log("Data Series length = " + dataSeries.count)


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
