import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.LFWQSP2641.qmlcomponents

Item {
    RowLayout {
        anchors.fill: parent
        GoBoardItem {
            id: goBoard
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        Button {
            text: "start"
            onClicked: {
                handler.start()
                enabled = false
            }
        }
    }


    GameBoardHandler {
        id: handler
        onBoardDataArrayUpdate: function(boardDataArray) {
            goBoard.setBoardData(boardDataArray)
        }
        onBestPointUpdate: function(stoneData) {
            goBoard.setBestMove(stoneData)
        }
    }
}
