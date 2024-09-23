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
            id: startGameButton
            text: "start"
            onClicked: {
                handler.startGame()
                startGameButton.enabled = false
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
        onClearBestPoint: {
            goBoard.clearBestMove()
        }
        onGameOver: {
            startGameButton.enabled = true
        }
        onToReset: {
            goBoard.clearBestMove()
            goBoard.clearBoardData()
        }
    }
}
