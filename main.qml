import QtQuick 2.12
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1

import org.freedesktop.gstreamer.GLVideoItem 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    x: 30
    y: 30
    color: "black"


    Rectangle {
        id: rect
        width: 30
        height: window.height
        z: 1000
        XAnimator on x {
            from: 0
            to: window.width + rect.width;
            duration: 200
            running: true
            loops: Animation.Infinite
        }
    }

    Item {
        anchors.fill: parent

        GstGLVideoItem {
            id: video
            objectName: "videoItem"
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
        }

        Rectangle {
            id: circle
            color: "red"
            anchors.bottom: video.bottom
            anchors.right: video.right
            anchors.margins: 15
            width : 50
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
