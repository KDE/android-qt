import Qt 4.6

Item {
    property alias x: item.x
    Item { id: item }

    function testFunction() { return 9; }
    property int test: testFunction();
}
