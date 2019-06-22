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

    Item {
        anchors.fill: parent

        GridLayout {
            id: lay
            rows: 1
            columns: rows
            anchors.fill: parent

            Repeater {
                model: lay.rows*lay.columns
                VideoElement {
                    id: videoElement
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    description: "gst-launch-1.0 -vc udpsrc address=0.0.0.0 port=5600 close-socket=false auto-multicast=true ! application/x-rtp, payload=96 ! rtph264depay ! decodebin3"
                }
            }
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
}
