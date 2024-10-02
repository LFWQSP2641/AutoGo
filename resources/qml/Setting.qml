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
                    GroupBox {
                        Layout.fillWidth: true
                        title: "Mumu增强模式"
                        ColumnLayout {
                            anchors.fill: parent
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "Mumu增强模式:"
                                }
                                CheckBox {
                                    Layout.fillWidth: true
                                    text: "开启"
                                    checked: Settings.mumuEnable === true
                                    onClicked: {
                                        Settings.mumuEnable = checked
                                        Settings.saveToFile()
                                    }
                                }
                            }
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
                            text: "KataGo模式(重启生效):"
                        }
                        RadioButton {
                            Layout.fillWidth: true
                            text: "Analysis"
                            checked: Settings.kataGoMode === "Analysis"
                            onClicked: {
                                Settings.kataGoMode = "Analysis"
                                Settings.saveToFile()
                            }
                        }
                        RadioButton {
                            Layout.fillWidth: true
                            text: "GTP"
                            checked: Settings.kataGoMode === "GTP"
                            onClicked: {
                                Settings.kataGoMode = "GTP"
                                Settings.saveToFile()
                            }
                        }
                    }
                    GroupBox {
                        Layout.fillWidth: true
                        title: "KataGo Analysis"
                        ColumnLayout {
                            anchors.fill: parent
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "KataGo Analysis参数:"
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    placeholderText: "不包括KataGo.exe"
                                    text: Settings.kataGoAnalysisCommand
                                    onTextChanged: Settings.kataGoAnalysisCommand = text
                                    onEditingFinished: Settings.saveToFile()
                                }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "配置文件存在search limit限制(将不会开启计时器):"
                                }
                                CheckBox {
                                    Layout.fillWidth: true
                                    checked: Settings.kataGoSearchLimit === true
                                    onClicked: {
                                        Settings.kataGoSearchLimit = checked
                                        Settings.saveToFile()
                                    }
                                }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                enabled: Settings.kataGoSearchLimit
                                Label {
                                    text: "基准maxVisits(20s内必下一手):"
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    text: Settings.kataGoMaxVisits
                                    onTextChanged: Settings.kataGoMaxVisits = parseInt(text)
                                    onEditingFinished: Settings.saveToFile()
                                }
                            }
                        }
                    }
                    GroupBox {
                        Layout.fillWidth: true
                        title: "KataGo GTP"
                        ColumnLayout {
                            anchors.fill: parent
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: "KataGo GTP参数:"
                                }
                                TextField {
                                    Layout.fillWidth: true
                                    placeholderText: "不包括KataGo.exe"
                                    text: Settings.kataGoGTPCommand
                                    onTextChanged: Settings.kataGoGTPCommand = text
                                    onEditingFinished: Settings.saveToFile()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
