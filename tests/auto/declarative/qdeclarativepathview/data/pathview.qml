import Qt 4.6

Rectangle {
    width: 240
    height: 320
    color: "#ffffff"
    resources: [
        Component {
            id: delegate
            Rectangle {
                id: wrapper
                objectName: "wrapper"
                height: 20
                width: 60
                color: "white"
                border.color: "black"
                Text {
                    text: index
                }
                Text {
                    x: 20
                    id: textName
                    objectName: "textName"
                    text: name
                }
                Text {
                    x: 40
                    id: textNumber
                    objectName: "textNumber"
                    text: number
                }
            }
        }
    ]
    PathView {
        id: view
        objectName: "view"
        width: 240
        height: 320
        model: testModel
        delegate: delegate
        snapPosition: 0.01
        path: Path {
            startY: 120
            startX: 160
            PathQuad {
                y: 120
                x: 80
                controlY: 330
                controlX: 100
            }
            PathLine {
                y: 160
                x: 20
            }
            PathCubic {
                y: 120
                x: 160
                control1Y: 0
                control1X: 100
                control2Y: 000
                control2X: 200
            }
        }
    }
}
