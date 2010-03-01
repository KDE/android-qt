import Qt 4.6

Rectangle {
    width: 400; height: 400
    Rectangle {
        id: myRect
        width: 100; height: 100
        color: "red"
    }
    MouseArea {
        id: clickable
        anchors.fill: parent
    }

    states: State {
        name: "state1"
        when: clickable.pressed
        PropertyChanges {
            target: myRect
            x: 50; y: 50
        }
        StateChangeScript {
            name: "setColor"
            script: myRect.color = "blue"
        }
    }

    transitions: Transition {
        SequentialAnimation {
            NumberAnimation { properties: "x"; easing.type: "InOutQuad" }
            ScriptAction { stateChangeScriptName: "setColor" }
            NumberAnimation { properties: "y"; easing.type: "InOutQuad" }
        }
    }
}
