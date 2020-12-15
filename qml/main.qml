import QtQuick 2.12
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1
import QtQuick.Layouts 1.12

import VideoElement 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    x: 30
    y: 30
    color: "black"

    VideoElement {
        id: videoElement
        objectName: "videoElement"
        anchors.fill: parent
    }

    Rectangle {
        id: circle
        color: "red"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 15
        width : 30
        height: width
        radius: width

        Timer {
            id: hidetimer
            interval: 300
            running: true
            repeat: true
            onTriggered: {
                circle.visible = !circle.visible
            }
        }
    }
}
