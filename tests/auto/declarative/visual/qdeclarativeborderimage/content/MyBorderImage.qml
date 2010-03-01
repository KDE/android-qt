import Qt 4.6

Item {
    property alias horizontalMode: image.horizontalTileMode
    property alias verticalMode: image.verticalTileMode
    property alias source: image.source
    property alias antialiased: image.smooth

    property int minWidth
    property int minHeight
    property int maxWidth
    property int maxHeight
    property int margin

    id: container
    width: 240; height: 240

    BorderImage {
        id: image; x: container.width / 2 - width / 2; y: container.height / 2 - height / 2

        width: SequentialAnimation {
            repeat: true
            NumberAnimation { from: container.minWidth; to: container.maxWidth; duration: 2000; easing.type: "InOutQuad"}
            NumberAnimation { from: container.maxWidth; to: container.minWidth; duration: 2000; easing.type: "InOutQuad" }
        }

        height: SequentialAnimation {
            repeat: true
            NumberAnimation { from: container.minHeight; to: container.maxHeight; duration: 2000; easing.type: "InOutQuad"}
            NumberAnimation { from: container.maxHeight; to: container.minHeight; duration: 2000; easing.type: "InOutQuad" }
        }

        border.top: container.margin
        border.left: container.margin
        border.bottom: container.margin
        border.right: container.margin
    }
}
