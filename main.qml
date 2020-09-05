import QtQuick 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480

    Loader {

        id: pageLoader
        source: "Home.qml"
        anchors.fill: parent

    }


}
