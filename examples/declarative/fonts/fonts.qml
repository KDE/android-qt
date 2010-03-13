import Qt 4.6

Rectangle {
    property string myText: "The quick brown fox jumps over the lazy dog."

    width: 800; height: 480
    color: "steelblue"

    FontLoader { id: fixedFont; name: "Courier" }
    FontLoader { id: localFont; source: "fonts/tarzenau-ocr-a.ttf" }
    FontLoader { id: webFont; source: "http://www.princexml.com/fonts/steffmann/Starburst.ttf" }

    Column {
        anchors.fill: parent; spacing: 15
        anchors.leftMargin: 10; anchors.rightMargin: 10
        Text {
            text: myText; color: "lightsteelblue"
            width: parent.width; elide: Text.ElideRight
            font.family: "Times"; font.pointSize: 42
        }
        Text {
            text: myText; color: "lightsteelblue"
            width: parent.width; elide: Text.ElideLeft
            font.family: "Times"; font.pointSize: 42
            font.capitalization: Font.AllUppercase
        }
        Text {
            text: myText; color: "lightsteelblue"
            width: parent.width; elide: Text.ElideMiddle
            font.family: fixedFont.name; font.pointSize: 42; font.weight: Font.Bold
            font.capitalization: Font.AllLowercase
        }
        Text {
            text: myText; color: "lightsteelblue"
            width: parent.width; elide: Text.ElideRight
            font.family: fixedFont.name; font.pointSize: 42; font.italic: true
            font.capitalization: Font.SmallCaps
        }
        Text {
            text: myText; color: "lightsteelblue"
            width: parent.width; elide: Text.ElideLeft
            font.family: localFont.name; font.pointSize: 42
            font.capitalization: Font.Capitalize
        }
        Text {
            text: {
                if (webFont.status == 1) myText
                else if (webFont.status == 2) "Loading..."
                else if (webFont.status == 3) "Error loading font"
            }
            color: "lightsteelblue"
            width: parent.width; elide: Text.ElideMiddle
            font.family: webFont.name; font.pointSize: 42
        }
    }
}
