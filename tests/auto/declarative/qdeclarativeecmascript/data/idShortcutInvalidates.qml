import Qt.test 1.0
import Qt 4.6

MyQmlObject {
    objectProperty: otherObject

    property var obj

    obj: QtObject {
        id: otherObject
    }
}
