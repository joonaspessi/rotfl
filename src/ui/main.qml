import QtQuick 2.0

Rectangle {
    id: root
    objectName: "cleanButton"
    property alias label: text.text
    property bool active: false
    signal toggled
    width: 149
    height: 30
    radius: 3
    color: active ? "red" : "green"
    border.width: 1
    Text { id: text; anchors.centerIn: parent; font.pixelSize: 14; text: active ? "Cleaning" : "Clean" }
    MouseArea {
        anchors.fill: parent
        onClicked: { active = !active; root.toggled() }
    }
}
