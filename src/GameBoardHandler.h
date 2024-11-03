#ifndef GAMEBOARDHANDLER_H
#define GAMEBOARDHANDLER_H

#include "GameData.h"
#include "StoneData.h"

#include <QObject>

class GameAnalyzer;
class GameInteractor;
class KatagoInteractor;

class GameBoardHandler : public QObject
{
    Q_OBJECT

public:
    explicit GameBoardHandler(QObject *parent = nullptr);
    ~GameBoardHandler();

    enum State
    {
        Stopped = 0,
        Playing,
        NavigatingToMain,
        WaitingMatching
    };
    Q_ENUM(State)

    enum Task
    {
        StopGame = -1,
        NoTask = 0,
        StartGame
    };
    Q_ENUM(Task)

    bool inited() const;

    GameBoardHandler::State state() const;

    GameBoardHandler::Task task() const;
    void setTask(GameBoardHandler::Task newTask);

    int getGameCount() const;

    bool getGameStarted() const;

    bool getContinuousPlayGame() const;
    void setContinuousPlayGame(bool newContinuousPlayGame);

public slots:
    void init();
    void startGame();
    void stopGame();
    void continuousStartGame();
    void setTimeMode(int timeMode);

protected:
    GameAnalyzer *m_gameAnalyzer;
    GameInteractor *m_gameInteractor;
    KatagoInteractor *m_katagoInteractor;

    QThread *m_gameAnalyzerThread;
    QThread *m_katagoInteractorThread;

    bool m_inited;
    GameBoardHandler::State m_state;
    GameBoardHandler::Task m_task;

    StoneData lastMoveStone;
    StoneData lastBestMoveStone;
    QTimer *m_checkTimer;

    int gameCount;
    bool gameStarted;

    bool continuousPlayGame;

    int m_timeMode;

    bool m_pauseReception;

protected slots:
    void connectSignals();

    void startTask();

    void handleGamePage(GameData::AppNavigation appNavigation);

    void checkStoneMove();

    void handleGameOpening(const GameData &gameData);

    void pauseGameAnalyze(const unsigned long &duration);

protected slots:
    void onInitFinished(bool success);

    void onGameAnalyzerDataUpdated(const GameData &gameData);
    void onKatagoBestMove(const StoneData &stoneData);

signals:
    void boardDataArrayUpdate(const QList<QList<int>> boardDataArray);
    void bestPointUpdate(const StoneData &stoneData);

    void initStarting();

    void clearBestPoint();

    void gameStarting();
    void gameOver();

    void startInitFinished(bool success);

    void checkingAppNavigation();

signals:
    void toStartInit();
    void toSetTimeMode(int timeMode);
    void toAnalyzeGameBoard();
    void toStopGame();
    void toKatagoMove(const BoardData &boardData);
    void toPauseAnalyze(const unsigned long &duration);
    void toAnalyzeReset();

signals:
    void taskChanged();

    void continuousPlayGameChanged();

private:
    Q_PROPERTY(bool inited READ inited CONSTANT FINAL)
    Q_PROPERTY(GameBoardHandler::State state READ state CONSTANT FINAL)
    Q_PROPERTY(GameBoardHandler::Task task READ task WRITE setTask NOTIFY taskChanged FINAL)
    Q_PROPERTY(int gameCount READ getGameCount CONSTANT FINAL)
    Q_PROPERTY(bool gameStarted READ getGameStarted CONSTANT FINAL)
    Q_PROPERTY(bool continuousPlayGame READ getContinuousPlayGame WRITE setContinuousPlayGame NOTIFY continuousPlayGameChanged FINAL)
};

#endif // GAMEBOARDHANDLER_H
