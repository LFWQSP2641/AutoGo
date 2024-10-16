#ifndef BOARDANALYZER_H
#define BOARDANALYZER_H

#include "BoardData.h"

#include <QObject>

class Screencaptor;
namespace cv
{
class Mat;
} // namespace cv

class BoardAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit BoardAnalyzer(QObject *parent = nullptr);

    enum AppNavigation
    {
        gameOverDialog,             // 对局结束和升级等有关闭按钮的弹窗
        levelUpDialog,              // 升级弹窗
        acceptCountingResultDialog, // 接受数子结果
        tipDialog,                  // 先手及跳过, 没有弹窗, 但是会影响棋子识别
        confirmDefeatDialog,        // 确认认输
        requestDrawDialog,          // 请求和棋
        requestCountingDialog,      // 请求数子
        requestUndoDialog,          // 请求悔棋
        requestRematchDialog,       // 请求重新匹配
        requestResumeBattleDialog,  // 请求续战
        cancelResumeBattleDialog,   // 取消续战
        otherDialog,                // 其他具有按钮的对话框
        playingPage,                // 对弈页面
        playingPageWithMove,        // 对弈页面且己方正在下棋
        analysisPage,               // 复盘页面
        pageWithBack,               // 任何有返回按键的页面
        durationChoiceDialog,       // 时长选择对话框
        matchDialog,                // 匹配对话框
        mainPage,                   // 主页面
        unknownPage
    };
    Q_ENUM(AppNavigation)

    static QHash<QString, QPoint> templateImagePoints;

    static BoardAnalyzer::AppNavigation appNavigationAnalyze(const cv::Mat &image);

public slots:
    BoardData analyzeBoard();

    void init();
    void resetBoardData();
    void analyzeIndefinitely();
    void stop();

    void startGame();

protected:
    Screencaptor *screencaptor;
    BoardData m_boardData;

    bool toStop;

    bool checkGameState(AppNavigation navigation);
    void waitForGameMatching();

    static StoneData::StoneColor getMyStoneColor(const cv::Mat &image);

    void isTurnToPlay(const cv::Mat &image);
    void getBoardArray(const cv::Mat &image);
    bool checkGameStatus(const cv::Mat &image);

signals:
    void myStoneColorUpdate(StoneData::StoneColor newMyStoneColor);
    void analyzeBoardData(const BoardData &data);
    void analyzeIndefinitelyData(const BoardData &data);
    void gameStarted();
    void lastMoveStone(const StoneData &lastMoveStoneData);
    void analyzeStoped();

    void toMatchGame();
    void toAcceptRequest();
    void toRejectRequest();
    void toCloseGameOverDialog();
    void toCloseLevelUpDialog();
    void toBackToMain();
    void toCloseRequest();
};

#endif // BOARDANALYZER_H
