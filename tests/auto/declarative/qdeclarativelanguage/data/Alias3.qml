import Test 1.0
import Qt 4.6

QtObject {
    property alias obj : otherObj
    property var child
    child: QtObject {
        id: otherObj
        property int myValue: 10
    }
}

