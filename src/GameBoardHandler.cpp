#include "GameBoardHandler.h"

#include "BoardAnalyzer.h"
#include "BoardInteractor.h"
#include "KatagoAnalysisInteractor.h"
#include "KatagoGTPInteractor.h"
#include "Settings.h"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QTimer>

GameBoardHandler::GameBoardHandler(QObject *parent)
    : QObject{ parent },
      boardAnalyzer(new BoardAnalyzer),
      boardInteractor(new BoardInteractor),
      boardAnalyzerThread(new QThread),
      boardInteractorThread(new QThread),
      katagoInteractorThread(new QThread),
      // timer(new QTimer),
      // timerThread(new QThread),
      inited(false)
{
    if (Settings::getSingletonSettings()->kataGoMode() == QStringLiteral("Analysis"))
        katagoInteractor = new KatagoAnalysisInteractor;
    else if (Settings::getSingletonSettings()->kataGoMode() == QStringLiteral("GTP"))
        katagoInteractor = new KatagoGTPInteractor;
    else
        qFatal() << Q_FUNC_INFO << QStringLiteral("kataGoMode is invalid");

    boardAnalyzer->moveToThread(boardAnalyzerThread);
    boardInteractor->moveToThread(boardInteractorThread);
    katagoInteractor->moveToThread(katagoInteractorThread);
    connect(boardAnalyzerThread, &QThread::finished, boardAnalyzer, &BoardAnalyzer::deleteLater);
    connect(boardInteractorThread, &QThread::finished, boardInteractor, &BoardInteractor::deleteLater);
    connect(katagoInteractorThread, &QThread::finished, katagoInteractor, &KatagoInteractor::deleteLater);

    // timer->setSingleShot(true);

    // timer->moveToThread(timerThread);
    // connect(timerThread, &QThread::finished, timer, &QTimer::deleteLater);

    connect(this, &GameBoardHandler::startInit, this, &GameBoardHandler::init);

    connect(boardAnalyzerThread, &QThread::started, boardAnalyzer, &BoardAnalyzer::init);
    connect(boardInteractorThread, &QThread::started, boardInteractor, &BoardInteractor::init);
    connect(katagoInteractorThread, &QThread::started, katagoInteractor, &KatagoInteractor::init);

    connect(katagoInteractor, &KatagoInteractor::initFinished, this, &GameBoardHandler::startInitFinished);

    connect(this, &GameBoardHandler::startInitFinished, this, &GameBoardHandler::onInitFinished);

    connect(this, &GameBoardHandler::toStartGame, boardAnalyzer, &BoardAnalyzer::startGame);
    connect(this, &GameBoardHandler::toStartGame, this, &GameBoardHandler::checkingAppNavigation);

    connect(this, &GameBoardHandler::toSetTimeMode, katagoInteractor, &KatagoInteractor::setTimeModeFromInt);
    connect(this, &GameBoardHandler::toSetTimeMode, boardInteractor, &BoardInteractor::setTimeModeFromInt);

    connect(boardAnalyzer, &BoardAnalyzer::toMatchGame, boardInteractor, &BoardInteractor::matchGame);
    connect(boardAnalyzer, &BoardAnalyzer::toAcceptRequest, boardInteractor, &BoardInteractor::acceptRequest);
    connect(boardAnalyzer, &BoardAnalyzer::toRejectRequest, boardInteractor, &BoardInteractor::rejectRequest);
    connect(boardAnalyzer, &BoardAnalyzer::toCloseGameOverDialog, boardInteractor, &BoardInteractor::closeGameOverDialog);
    connect(boardAnalyzer, &BoardAnalyzer::toCloseLevelUpDialog, boardInteractor, &BoardInteractor::closeLevelUpDialog);
    connect(boardAnalyzer, &BoardAnalyzer::toBackToMain, boardInteractor, &BoardInteractor::backToMain);
    connect(boardAnalyzer, &BoardAnalyzer::toCloseRequest, boardInteractor, &BoardInteractor::closeRequest);

    connect(boardAnalyzer, &BoardAnalyzer::gameStarted, this, &GameBoardHandler::analyzeIndefinitelyDelay);
    connect(boardAnalyzer, &BoardAnalyzer::gameStarted, this, &GameBoardHandler::gameStarted);

    connect(this, &GameBoardHandler::toPlay, boardInteractor, QOverload<const QPoint &>::of(&BoardInteractor::moveStone));
    connect(this, &GameBoardHandler::toStartAnalyzeIndefinitely, boardAnalyzer, &BoardAnalyzer::analyzeIndefinitely);
    connect(boardAnalyzer, &BoardAnalyzer::myStoneColorUpdate, this, &GameBoardHandler::gameStartedHandle);

    connect(boardAnalyzer, &BoardAnalyzer::analyzeIndefinitelyData, this, &GameBoardHandler::onStoneMoved);
    connect(boardAnalyzer, &BoardAnalyzer::analyzeIndefinitelyData, katagoInteractor, &KatagoInteractor::move);

    connect(boardInteractor, &BoardInteractor::moveFinished, this, &GameBoardHandler::clearBestPoint);

    // connect(this, &GameBoardHandler::toStartTimer, timer, QOverload<int>::of(&QTimer::start));
    // connect(timer, &QTimer::timeout, katagoInteractor, &KatagoInteractor::getBestMove);
    // connect(timer, &QTimer::timeout, timer, []
    //         { qDebug() << QStringLiteral("time out") << QDateTime::currentMSecsSinceEpoch(); });
    connect(katagoInteractor, &KatagoInteractor::bestMoveUpdate, this, &GameBoardHandler::bestPointUpdate);
    connect(katagoInteractor, &KatagoInteractor::bestMove, boardInteractor, QOverload<const StoneData &>::of(&BoardInteractor::moveStone));

    connect(this, &GameBoardHandler::toAcceptRequest, boardInteractor, &BoardInteractor::acceptRequest);
    connect(this, &GameBoardHandler::toRejectRequest, boardInteractor, &BoardInteractor::rejectRequest);

    connect(this, &GameBoardHandler::toStopGame, katagoInteractor, &KatagoInteractor::stopAnalyze);
    connect(this, &GameBoardHandler::toStopGame, boardAnalyzer, &BoardAnalyzer::stop);

    connect(this, &GameBoardHandler::gameOver, katagoInteractor, &KatagoInteractor::stopAnalyze);
    connect(this, &GameBoardHandler::gameOver, boardAnalyzer, &BoardAnalyzer::stop);
    // connect(this, &GameBoardHandler::gameOver, timer, &QTimer::stop);

    connect(this, &GameBoardHandler::toReset, boardAnalyzer, &BoardAnalyzer::resetBoardData);
    connect(this, &GameBoardHandler::toReset, katagoInteractor, &KatagoInteractor::clearBoard);
}

void GameBoardHandler::startGame()
{
    if (inited)
    {
        emit toReset();
        emit toStartGame();
    }
    else
    {
        emit startInit();
    }
}

void GameBoardHandler::stopGame()
{
    qDebug() << Q_FUNC_INFO;
    emit toStopGame();
}

void GameBoardHandler::continuousStartGame()
{
    QTimer::singleShot(3000, this, &GameBoardHandler::startGame);
}

void GameBoardHandler::setTimeMode(int timeMode)
{
    qDebug() << Q_FUNC_INFO << timeMode;
    emit toSetTimeMode(timeMode);
}

void GameBoardHandler::init()
{
    boardAnalyzerThread->start();
    boardInteractorThread->start();
    katagoInteractorThread->start();
    // timerThread->start();
}

void GameBoardHandler::gameStartedHandle(StoneData::StoneColor myStoneColor)
{
    qDebug() << Q_FUNC_INFO << myStoneColor;
    if (myStoneColor == StoneData::StoneColor::Black)
    {
        emit toPlay(QPoint(3, 3));
    }
    else if (myStoneColor == StoneData::StoneColor::None)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("myStoneColor is None");
        return;
    }
    emit toStartAnalyzeIndefinitely();
}

void GameBoardHandler::onStoneMoved(const BoardData &boardData)
{
    if (!boardData.hasUnexpected())
        emit boardDataArrayUpdate(boardData.getBoardDataArray());

    if (boardData.getRequestCounting() || boardData.getRequestUndo())
    {
        emit toAcceptRequest();
    }
    else if (boardData.getRequestDraw())
    {
        emit toRejectRequest();
    }
    else if (boardData.getGameOver())
    {
        emit gameOver();
    }

    // if (boardData.getNeedMove())
    // {
    //     const auto msec(Util::generateTanhRandom(3000, 7000));
    //     qDebug() << Q_FUNC_INFO << QStringLiteral("timer starting, msec:") << msec << QDateTime::currentMSecsSinceEpoch();
    //     if (timer->isActive())
    //         qWarning() << "timer had been running";
    //     emit toStartTimer(msec);
    // }
    // else
    // {
    //     qDebug() << Q_FUNC_INFO << QStringLiteral("not my turn");
    // }
}

void GameBoardHandler::analyzeIndefinitelyDelay()
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(2000, this, &::GameBoardHandler::toStartAnalyzeIndefinitely);
}

void GameBoardHandler::onInitFinished()
{
    inited = true;
    emit toStartGame();
}
