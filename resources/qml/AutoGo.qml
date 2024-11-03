import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.LFWQSP2641.qmlcomponents

Item {
    GoBoardItem {
        id: goBoard
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: Math.min(parent.width, parent.height)
        height: width
    }

    RowLayout {
        anchors.left: goBoard.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        ColumnLayout {
            Flickable {
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                contentHeight: gameLogArea.height
                onContentHeightChanged: Qt.callLater(() => contentY = contentHeight - height)
                TextArea {
                    id: gameLogArea
                    width: parent.width
                    placeholderText: "日志"
                    readOnly: true

                    function log(text) {
                        gameLogArea.append(Qt.formatDateTime(new Date(), "hh:mm:ss.zzz") + " " + text)
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: "时长选择:"
                }
                ComboBox {
                    id: durationComboBox
                    Layout.fillWidth: true
                    model: ["短(1分钟)", "中(5分钟)", "长(20分钟)"]
                    currentIndex: 0
                    onCurrentIndexChanged: {
                        interactor.setTimeMode(currentIndex)
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Switch {
                    id: continuousPlaySwitch
                    Layout.fillWidth: true
                    text: "连续对弈"
                    onCheckedChanged: {
                        interactor.gameBoardHandler.continuousPlayGame = checked
                    }
                }
                Button {
                    id: startGameButton
                    Layout.fillWidth: true
                    text: "AI对弈"
                    onClicked: {
                        startGameButton.enabled = false
                        durationComboBox.enabled = false
                        if (!interactor.gameBoardHandler.inited)
                        {
                            interactor.init()
                            return
                        }
                        interactor.startGame()
                    }
                }
                Button {
                    id: stopGameButton
                    Layout.fillWidth: true
                    text: "停止"
                    onClicked: {
                        interactor.stopGame()
                        startGameButton.enabled = true
                        startGameButton.enabled = true
                    }
                }
            }
        }
    }

    AutoGoInteractor {
        id: interactor
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
            gameLogArea.log("对弈结束")
            gameLogArea.log("对弈次数: " + interactor.gameBoardHandler.gameCount)
            if(continuousPlaySwitch.checked)
            {
                gameLogArea.log("连续对弈")
            }
            else
            {
                startGameButton.enabled = true
                durationComboBox.enabled = true
            }
        }
        onInitStarting: {
            gameLogArea.log("开始初始化")
        }
        onStartInitFinished: function(success) {
            if(success)
            {
                gameLogArea.log("初始化成功")
                interactor.startGame()
            }
            else
            {
                startGameButton.enabled = true
                durationComboBox.enabled = true
                gameLogArea.log("初始化失败")
            }
        }
        onCheckingAppNavigation: {
            gameLogArea.log("检查所处位置")
        }
        onGameStarting: {
            gameLogArea.log("清空棋盘")
            goBoard.clearBestMove()
            goBoard.clearBoardData()
            gameLogArea.log("开始对弈")
        }
    }
}
