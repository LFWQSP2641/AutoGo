import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.LFWQSP2641.qmlcomponents

Item {
    id: root
    property int clearCacheGameCount: 2

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
                id: flickable
                Layout.fillHeight: true
                Layout.fillWidth: true
                clip: true
                contentHeight: gameLogArea.height
                TextArea {
                    id: gameLogArea
                    width: parent.width
                    placeholderText: "日志"
                    readOnly: true

                    function log(text) {
                        gameLogArea.output(Qt.formatDateTime(new Date(), "hh:mm:ss.zzz") + " " + text)
                    }

                    function output(text) {
                        gameLogArea.append(text)
                        let contentY = flickable.contentHeight - flickable.height
                        flickable.contentY = contentY > 0 ? contentY : 0
                    }
                }
                // onContentHeightChanged: {
                //     console.log("contentHeight:" + contentHeight)
                //     Qt.callLater(() => contentY = contentHeight - height)
                // }
                // onContentYChanged: {
                //     console.log("contentY: " + contentY)
                //     Qt.callLater(() => contentY = contentHeight - height)
                // }
            }
            RowLayout {
                Layout.fillWidth: true
                Button {
                    text: "清除缓存"
                    onClicked: {
                        interactor.clearKataGoCache()
                    }
                }
                Label {
                    text: "自动清除缓存:"
                }
                TextField {
                    id: clearCacheTurnTextField
                    Layout.fillWidth: true
                    placeholderText: "对局数 (0为不清除)"
                    text: "2"
                    validator: IntValidator { bottom: 0 }
                    onEditingFinished: {
                        let count = parseInt(text)
                        if (count != NaN)
                        {
                            root.clearCacheGameCount = count
                        }
                        else
                        {
                            gameLogArea.log("输入无效")
                            text = "2"
                            root.clearCacheGameCount = 2
                        }
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
            root.clearCacheGameCount -= 1
            if(root.clearCacheGameCount == 0)
            {
                gameLogArea.log("清除缓存")
                interactor.clearKataGoCache()
                root.clearCacheGameCount = parseInt(clearCacheTurnTextField.text)
            }
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
        onErrorOccurred: function(error, formatted) {
            if(formatted)
            {
                gameLogArea.output(error)
            }
            else
            {
                gameLogArea.log("错误: " + error)
            }
		}
        onLogMessage: function(message, formatted) {
            if(formatted)
			{
                gameLogArea.output(message)
			}
            else
			{
				gameLogArea.log(message)
			}
		}
    }
}
