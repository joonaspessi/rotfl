import QtQuick 2.1
import QtGraphicalEffects 1.0
Item {
    width: 400
    height: 380


    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    RectangularGlow {
        id: effect
        anchors.fill: battery
        glowRadius: 2
        spread: 0.3
        color: "black"
        cornerRadius: battery.radius + glowRadius
    }

    Rectangle {
        id: battery
        color: "white"

        anchors.left: parent.left
        anchors.leftMargin: parent.width/10
        anchors.top: parent.top
        anchors.topMargin: parent.height/8
        width: Math.round(parent.width / 1.2)
        height: Math.round(parent.height / 6)
        radius: 10
    }

    Rectangle {
        id: batteryLevel
        color: "#55ff55"
        anchors.left: battery.left
        anchors.top: battery.top
        width: battery.width
        height: battery.height
        radius: 10

        Behavior on width { SpringAnimation { spring: 1; damping: 0.1 } }

        PropertyAnimation { id: animation; target: batteryLevel; property: "width"; to: 100; duration: 500 }

        MouseArea { anchors.fill: parent; onClicked: {
            batteryLevel.width -= battery.width*0.1;
            batteryLeveltext.percentage -= 10;
            batteryLabel.mAh -= 2600*0.1;


                if(batteryLevel.width <= battery.width*0.8) {
                    batteryLevel.color = "#55ff55"
                }
                if(batteryLevel.width < battery.width*0.5) {
                    batteryLevel.color = "yellow"
                }
                if(batteryLevel.width <= battery.width*0.2) {
                    batteryLevel.color = "red"
                }

            }}
        focus: true;
        Keys.onPressed: {
            if(event.key == Qt.Key_A) {
                batteryLevel.width = battery.width;
                batteryLeveltext.percentage = 100
                batteryLevel.color = "#55ff55"
            }
        }



    }
    Text {
        id: batteryLeveltext
        property int percentage: 100
        text: percentage + " %"
        anchors.centerIn: battery
    }
    Text {
        id: batteryLabel
        property int mAh: 2600
        text: 'battery level '  + mAh + 'mAh'
        anchors.bottom: battery.top
        anchors.bottomMargin: 5
        anchors.horizontalCenter: battery.horizontalCenter
    }


}

