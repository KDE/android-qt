//![0]
import Qt 4.6

Item { 
    id:block
    property int type: 0
    
    Image { id: img
        source: {
            if(type == 0){
                "../shared/pics/redStone.png";
            } else if(type == 1) {
                "../shared/pics/blueStone.png";
            } else {
                "../shared/pics/greenStone.png";
            }
        }
        anchors.fill: parent
    }
}
//![0]
