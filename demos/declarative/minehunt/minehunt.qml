import Qt 4.6

Item {
    id: field
    width: 370
    height: 480

    property int clickx : 0
    property int clicky : 0

    resources: [
        Component {
            id: tile
            Flipable {
                id: flipable
                width: 40
                height: 40
                property int angle: 0;
                transform: Rotation {
                    origin.x: 20
                    origin.y: 20
                    axis.x: 1
                    axis.z: 0
                    angle: flipable.angle;
                }
                front: Image {
                    source: "pics/front.png"
                    width: 40
                    height: 40
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "pics/flag.png"
                        opacity: model.hasFlag
                        opacity: Behavior {
                            NumberAnimation {
                                property: "opacity"
                                duration: 250
                            }
                        }
                    }
                }
                back: Image {
                    source: "pics/back.png"
                    width: 40
                    height: 40
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.hint
                        color: "white"
                        font.bold: true
                        opacity: !model.hasMine && model.hint > 0
                    }
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        source: "pics/bomb.png"
                        opacity: model.hasMine
                    }
                    Explosion {
                        id: expl
                    }
                }
                states: [
                    State {
                        name: "back"
                        when: model.flipped
                        PropertyChanges { target: flipable; angle: 180 }
                    }
                ]
                transitions: [
                    Transition {
                        SequentialAnimation {
                            PauseAnimation {
                                duration: {
                                    var ret;
                                    if(flipable.parent != null)
                                        ret = Math.abs(flipable.parent.x-field.clickx)
                                            + Math.abs(flipable.parent.y-field.clicky);
                                    else
                                        ret = 0;
                                    if (ret > 0) {
                                        if (model.hasMine && model.flipped) {
                                            ret*3;
                                        } else {
                                            ret;
                                        }
                                    } else {
                                        0;
                                    }
                                }
                            }
                            NumberAnimation {
                                easing.type: "InOutQuad"
                                properties: "angle"
                            }
                            ScriptAction{
                                script: if(model.hasMine && model.flipped){expl.explode = true;}
                            }
                        }
                    }
                ]
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onPressed: {
                        field.clickx = flipable.parent.x;
                        field.clicky = flipable.parent.y;
                        var row = Math.floor(index/9);
                        var col = index - (Math.floor(index/9) * 9);
                        if (mouse.button==undefined || mouse.button==Qt.RightButton) {
                            flag(row,col);
                        } else {
                            flip(row,col);
                        }
                    }
                }
            }
        }
    ]
    Image {
        source: "pics/No-Ones-Laughing-3.jpg"
        fillMode: Image.Tile
    }
    Description {
        text: "Use the 'minehunt' executable to run this demo!"
        width: 300
        opacity: tiles?0:1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    Repeater {
        id: repeater
        model: tiles
        x: 1
        y: 1
        Component {
            Loader {
                sourceComponent: tile
                x: (index - (Math.floor(index/9) * 9)) * 41
                y: Math.floor(index/9) * 41
            }
        }
    }
    Row {
        id: gamedata
       // width: 370
       // height: 100
        y: 400
        x: 20
        spacing: 20
        Column {
            spacing: 2
            width: childrenRect.width
            Image {
         //       x: 100
         //       y: 20
                source: "pics/bomb-color.png"
            }
            Text {
         //       x: 100
         //       y: 60
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                text: numMines
            }
        }
        Column {
            spacing: 2
            width: childrenRect.width
            Image {
         //       x: 140
         //       y: 20
                source: "pics/flag-color.png"
            }
            Text {
         //       x: 140
         //       y: 60
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                text: numFlags
            }
        }
    }
    Image {
        y: 390
        anchors.right: field.right
        anchors.rightMargin: 20
        source: isPlaying ? 'pics/face-smile.png' : hasWon ? 'pics/face-smile-big.png': 'pics/face-sad.png'
        MouseArea {
            anchors.fill: parent
            onPressed: { reset() }
        }
    }
}
