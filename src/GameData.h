#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "BoardData.h"

#include <QObject>

class GameData
{
    Q_GADGET
    friend class GameAnalyzer;

public:
    GameData();
    explicit GameData(QStringView uuid);
    GameData(const GameData &other);
    GameData &operator=(const GameData &other);

    bool operator==(const GameData &other) const;
    bool operator!=(const GameData &other) const;

    enum AppNavigation
    {
        unknownPage = -1,
        playingPage = 0,                 // 对弈页面
        playingPageWithMove,             // 对弈页面且己方正在下棋
        analysisPage,                    // 复盘页面
        pageWithBack,                    // 任何有返回按键的页面
        durationChoiceDialog,            // 时长选择对话框
        matchDialog,                     // 匹配对话框
        mainPage,                        // 主页面
        gameOverDialog,                  // 对局结束和升级等有关闭按钮的弹窗
        levelUpDialog,                   // 升级弹窗
        acceptCountingResultDialog,      // 接受数子结果
        tipDialog,                       // 先手及跳过, 没有弹窗, 但是会影响棋子识别
        confirmDefeatDialog,             // 确认认输
        requestDrawDialog,               // 请求和棋
        requestCountingDialog,           // 请求数子
        requestIntelligentRefereeDialog, // 请求智能裁判
        intelligentRefereeDialogFailed,  // 智能裁判失败
        requestUndoDialog,               // 请求悔棋
        requestRematchDialog,            // 请求重新匹配
        requestResumeBattleDialog,       // 请求续战
        cancelResumeBattleDialog,        // 取消续战
        ReconnectSuccessAndFinishDialog, // 重连成功并结束对局
        AIPKDialog,                      // AI PK
        otherDialog                      // 其他具有按钮的对话框
    };
    Q_ENUM(AppNavigation)

    QString uuid() const;

    BoardData boardData() const;

    AppNavigation appNavigation() const;

    StoneData::StoneColor myStoneColor() const;

    bool needMove() const;

protected:
    QString m_uuid;
    BoardData m_boardData;
    AppNavigation m_appNavigation;
    StoneData::StoneColor m_myStoneColor;
    bool m_needMove;

private:
    Q_PROPERTY(QString uuid READ uuid CONSTANT FINAL)
    Q_PROPERTY(BoardData boardData READ boardData CONSTANT FINAL)
    Q_PROPERTY(AppNavigation appNavigation READ appNavigation CONSTANT FINAL)
    Q_PROPERTY(StoneData::StoneColor myStoneColor READ myStoneColor CONSTANT FINAL)
    Q_PROPERTY(bool needMove READ needMove CONSTANT FINAL)
};

Q_DECLARE_METATYPE(GameData)

#endif // GAMEDATA_H
