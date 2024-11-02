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
                        handler.setTimeMode(currentIndex)
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
                        handler.setContinuousPlayGame(checked)
                    }
                }
                Button {
                    id: startGameButton
                    Layout.fillWidth: true
                    text: "AI对弈"
                    onClicked: {
                        startGameButton.enabled = false
                        durationComboBox.enabled = false
                        if (!handler.inited)
                        {
                            handler.init()
                            return
                        }
                        handler.startGame()
                    }
                }
                Button {
                    id: stopGameButton
                    Layout.fillWidth: true
                    text: "停止"
                    onClicked: {
                        handler.stopGame()
                        startGameButton.enabled = true
                        startGameButton.enabled = true
                    }
                }
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
            gameLogArea.append("对弈结束")
            if(continuousPlaySwitch.checked)
            {
                gameLogArea.append("连续对弈")
            }
            else
            {
                startGameButton.enabled = true
                durationComboBox.enabled = true
            }
        }
        onInitStarting: {
            gameLogArea.append("开始初始化")
        }
        onStartInitFinished: function(success) {
            if(success)
            {
                gameLogArea.append("初始化成功")
                handler.startGame()
            }
            else
            {
                startGameButton.enabled = true
                durationComboBox.enabled = true
                gameLogArea.append("初始化失败")
            }
        }
        onCheckingAppNavigation: {
            gameLogArea.append("检查所处位置")
        }
        onGameStarting: {
            gameLogArea.append("清空棋盘")
            goBoard.clearBestMove()
            goBoard.clearBoardData()
            gameLogArea.append("开始对弈")
        }
    }
}
