import Qt 4.6

Rectangle {
    id: wrapper
    width: 600
    height: 400

    Rectangle {
        id: redRect
        width: 100; height: 100
        color: Qt.rgba(1,0,0)
        x: SequentialAnimation {
            running: false
            NumberAnimation { objectName: "MyAnim"; running: true }
        }

    }

}
