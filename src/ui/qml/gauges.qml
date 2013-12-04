import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    width: 400
    height: 380

    Rectangle {
        id : backgroundcarbon
        width: 400
        height:380
        Image {
            anchors.fill: parent
            source: "../graphics/carbon.png"
        }
    }


    Rectangle {
        id: speedgauge
        property int needlezero : -117
        property int needle100 : 123
        property int needleangle : needlezero
        width: 200
        height: 200
        anchors.left : parent.left
        anchors.leftMargin: 2
        color : "transparent"
        Image {
            anchors.fill: parent
            source: "../graphics/defaultgauge.png"
            sourceSize.width: 200
            sourceSize.height: 200
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -30

            source: "../graphics/needleminicrop.png"
            sourceSize.width: 28
            sourceSize.height: 93

            transform:
                Rotation {
                id:rot
                // Transform origin is the middle point of the lower border
                origin {
                    x: 14
                    y: 80
                }

                //angle: -117
                angle: speedgauge.needleangle
                Behavior on angle { SpringAnimation { spring: 10; damping: 0.1 } }
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                //rot.angle = (speedgauge.needle100 + -1*(speedgauge.needlezero))*0.85 + speedgauge.needlezero
                rot.angle += 10;

                if(rot.angle >= speedgauge.needle100) {
                    rot.angle = speedgauge.needlezero;
                }
            }
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.6
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "speed"
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.68
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "50 cm/s"
        }

    }


    Rectangle {
        id: directiongauge
        color: "transparent"
        width: 200
        height: 200
        anchors.right: parent.right
        anchors.rightMargin: 2
        property int needlezero : 0
        property int needle100 : 123
        property int needleangle : needlezero



        Image {
            anchors.fill: parent
            source: "../graphics/compass.png"
            sourceSize.width: 200
            sourceSize.height: 200
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -30

            source: "../graphics/needleminicrop.png"
            sourceSize.width: 28
            sourceSize.height: 93

            transform:
                Rotation {
                id:rotationcompass
                // Transform origin is the middle point of the lower border
                origin {
                    x: 14
                    y: 80
                }

                //angle: -117
                angle: directiongauge.needleangle
                Behavior on angle { SpringAnimation { spring: 10; damping: 0.1 } }
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                rotationcompass.angle += 90;
            }
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.6
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "compass"
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.68
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "45 Deg"
        }
    }

    Rectangle {
        id: odometergauge
        color: "transparent"
        width: 180
        height: 180
        anchors.bottom: parent.bottom
        anchors.bottomMargin : 4
        anchors.right: parent.right
        anchors.rightMargin: 21



        Image {
            anchors.fill: parent
            source: "../graphics/odometer.png"
            sourceSize.width: 200
            sourceSize.height: 200
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -30

            source: "../graphics/needleminicrop.png"
            sourceSize.width: 28
            sourceSize.height: 93

            transform:
                Rotation {
                id:odometermeter
                // Transform origin is the middle point of the lower border
                origin {
                    x: 14
                    y: 80
                }

                //angle: -117
                angle: 0
                Behavior on angle { SpringAnimation { spring: 10; damping: 0.1 } }
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {

                odometermeter.angle += 10;
            }
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.6
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "odometer"
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.67
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "67m"
        }
    }

    Rectangle {
        id: batterygauge
        color: "transparent"
        width: 180
        height: 180
        anchors.bottom: parent.bottom
        anchors.bottomMargin : 4
        anchors.left: parent.left
        anchors.leftMargin: 21


        Image {
            anchors.fill: parent
            source: "../graphics/defaultgauge.png"
            sourceSize.width: 200
            sourceSize.height: 200
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -30

            source: "../graphics/needleminicrop.png"
            sourceSize.width: 28
            sourceSize.height: 93

            transform:
                Rotation {
                id:rotationbattery
                // Transform origin is the middle point of the lower border
                origin {
                    x: 14
                    y: 80
                }

                //angle: -117
                angle: 0
                Behavior on angle { SpringAnimation { spring: 10; damping: 0.1 } }
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {

                rotationbattery.angle += 10;
            }
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.6
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "battery %"
        }
        Text {
            anchors.top: parent.top
            anchors.topMargin: parent.width*0.68
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 1
            color : "white"
            text: "62 mAh"
        }

    }
}
