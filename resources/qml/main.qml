import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 800
    height: 500
    visible: true
    title: "AutoGo"

    ColumnLayout {
        anchors.fill: parent
        TabBar {
            id: swipeViewTabBar
            Layout.fillWidth: true
            currentIndex: swipeView.currentIndex
            TabButton {
                text: "自动"
            }
            TabButton {
                text: "设置"
            }
        }

        SwipeView {
            id: swipeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: swipeViewTabBar.currentIndex

            AutoGo {
                id: autogo
            }
            Setting {
                id: setting
            }
        }
    }
}
