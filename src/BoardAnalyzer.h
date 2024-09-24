#ifndef BOARDANALYZER_H
#define BOARDANALYZER_H

#include "BoardData.h"
#include "InteractiveInterface/MumuScreencaptor.h"

#include <QObject>

class BoardAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit BoardAnalyzer(QObject *parent = nullptr);

public slots:
    BoardData analyzeBoard();

    StoneData::StoneColor checkMyStoneColor();

    void init();
    void resetBoardData();
    void analyzeIndefinitely();
    void stop();

    void startGame();

protected:
    MumuScreencaptor *screencaptor;
    BoardData m_boardData;

    bool toStop;

    bool checkGameState(const cv::Mat &image);
    void waitForGameMatching();

    static StoneData::StoneColor getMyStoneColor(const cv::Mat &image);

    static void isTurnToPlay(const cv::Mat &image, BoardData &boardData);
    static void getBoardArray(const cv::Mat &image, BoardData &boardData);
    static bool checkGameStatus(const cv::Mat &image, BoardData &boardData);

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
    void toBreakToMain();
};

#endif // BOARDANALYZER_H
