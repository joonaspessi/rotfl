import QtQuick 2.1

Rectangle {
    gradient: Gradient {
        GradientStop { position: 0.0; color: "yellow" }
        GradientStop { position: 1.0; color: "green" }
    }
    width: 400
    height: 380

    Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: "lightsteelblue" }
            GradientStop { position: 1.0; color: "blue" }
        }
        width: 40
        height: 40
        anchors.left: parent
    }

    Text {
        text: "This is qml"
        anchors.centerIn: parent
    }

}
