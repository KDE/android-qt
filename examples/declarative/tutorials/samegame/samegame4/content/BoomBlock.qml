import Qt 4.6

Item { id:block
    property int type: 0
    property bool dying: false
    //![1]
    property bool spawned: false
    property int targetX: 0
    property int targetY: 0

    x: SpringFollow { enabled: spawned; source: targetX; spring: 2; damping: 0.2 }
    y: SpringFollow { source: targetY; spring: 2; damping: 0.2 }
    //![1]

    //![2]
    Image { id: img
        source: {
            if(type == 0){
                "pics/redStone.png";
            } else if(type == 1) {
                "pics/blueStone.png";
            } else {
                "pics/greenStone.png";
            }
        }
        opacity: 0
        opacity: Behavior { NumberAnimation { properties:"opacity"; duration: 200 } }
        anchors.fill: parent
    }
    //![2]

    //![3]
    Particles { id: particles
        width:1; height:1; anchors.centerIn: parent;
        emissionRate: 0;
        lifeSpan: 700; lifeSpanDeviation: 600;
        angle: 0; angleDeviation: 360;
        velocity: 100; velocityDeviation:30;
        source: {
            if(type == 0){
                "pics/redStar.png";
            } else if (type == 1) {
                "pics/blueStar.png";
            } else {
                "pics/greenStar.png";
            }
        }
    }
    //![3]

    //![4]
    states: [
        State{ name: "AliveState"; when: spawned == true && dying == false
            PropertyChanges { target: img; opacity: 1 }
        },
        State{ name: "DeathState"; when: dying == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 0 }
            StateChangeScript { script: block.destroy(1000); }
        }
    ]
    //![4]
}
