import Qt 4.6

Rectangle {
    id: container
    width: 200; height: 200
    Rectangle {
        id: myRect
        objectName: "MyRect"
        color: "green";
        anchors.left: parent.left
        anchors.right: rightGuideline.left
        anchors.top: topGuideline.top
        anchors.bottom: container.bottom
    }
    Item { id: leftGuideline; x: 10 }
    Item { id: rightGuideline; x: 150 }
    Item { id: topGuideline; y: 10 }
    Item { id: bottomGuideline; y: 150 }
    Item { id: topGuideline2; y: 50 }
    Item { id: bottomGuideline2; y: 175 }

    MouseArea {
        id: wholeArea
        anchors.fill: parent
        onClicked: {
            if (container.state == "") {
                container.state = "reanchored";
            } else if (container.state == "reanchored") {
                container.state = "reanchored2";
            } else if (container.state == "reanchored2")
                container.state = "reanchored";
        }
    }

    states: [ State {
        name: "reanchored"
        AnchorChanges {
            target: myRect;
            left: leftGuideline.left
            right: container.right
            top: container.top
            bottom: bottomGuideline.bottom
        }
    }, State {
        name: "reanchored2"
        AnchorChanges {
            target: myRect;
            reset: "left, right"
            top: topGuideline2.top
            bottom: bottomGuideline2.bottom
        }
    }]

    transitions: Transition {
        NumberAnimation { properties: "x,y,width,height" }
    }

    MouseArea {
        width: 50; height: 50
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onClicked: {
            container.state = "";
        }
    }

    state: "reanchored"
}
