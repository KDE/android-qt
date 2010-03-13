import Qt 4.6

Item {
    property bool explode : false

    Particles {
        id: particles
        width: 40
        height: 40
        lifeSpan: 1000
        lifeSpanDeviation: 0
        source: "pics/star.png"
        count: 0
        angle: 270
        angleDeviation: 360
        velocity: 100
        velocityDeviation: 20
        z: 100
        opacity: 1
    }
    states: [ State { name: "exploding"; when: explode == true
            StateChangeScript {script: particles.burst(200); }
        }
    ]

}
