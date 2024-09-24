import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Settings

Item {
    Flickable {
        anchors.fill: parent
        clip: true
        contentHeight: settingItemsLayout.height
        ColumnLayout {
            id: settingItemsLayout
            width: parent.width
            GroupBox {
                Layout.fillWidth: true
                title: "虚拟器连接相关"
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "Mumu安装路径:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.mumuPath
                            onTextChanged: Settings.mumuPath = text
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "Mumu实例号:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.mumuIndex
                            onTextChanged: Settings.mumuIndex = parseInt(text)
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "Mumu DisplayId:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.mumuDisplayId
                            onTextChanged: Settings.mumuDisplayId = parseInt(text)
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "adb路径:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.adbPath
                            onTextChanged: Settings.adbPath = text
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "adb被连接方:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.adbSerial
                            onTextChanged: Settings.adbSerial = text
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                }
            }
            GroupBox {
                Layout.fillWidth: true
                title: "围棋AI相关"
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "KataGo路径:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            text: Settings.kataGoPath
                            onTextChanged: Settings.kataGoPath = text
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: "KataGo参数:"
                        }
                        TextField {
                            Layout.fillWidth: true
                            placeholderText: "不包括KataGo.exe"
                            text: Settings.kataGoCommand
                            onTextChanged: Settings.kataGoCommand = text
                            onEditingFinished: Settings.saveToFile()
                        }
                    }
                }
            }
        }
    }
}
