import Qt 4.6

Item {
    width: 480; height: 320

    Loader {
        y: 320; rotation: -90
        transformOrigin: Item.TopLeft
        source: "flickr-mobile.qml"
    }
}
