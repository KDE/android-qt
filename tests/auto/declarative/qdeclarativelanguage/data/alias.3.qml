import Qt 4.6

QtObject {
   property var other
   other: Alias { id: myAliasObject }

   property alias value: myAliasObject.aliasValue
   property alias value2: myAliasObject.value
}

