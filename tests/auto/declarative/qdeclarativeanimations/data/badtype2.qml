import Qt 4.6

Rectangle {
    width: 240
    height: 320
    Rectangle {
        color: "red"
        width: 50; height: 50
        x: 100; y: 100
        x: NumberAnimation { from: "blue"; to: "green"; }
    }
}
