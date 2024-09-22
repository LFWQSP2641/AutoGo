#include "GameBoardHandler.h"

#include "BoardAnalyzer.h"
#include "BoardInteractor.h"
#include "KatagoInteractor.h"

#include <QDebug>
#include <QThread>
#include <QTimer>

GameBoardHandler::GameBoardHandler(QObject *parent)
    : QObject{ parent },
      boardAnalyzer(new BoardAnalyzer),
      boardInteractor(new BoardInteractor),
      katagoInteractor(new KatagoInteractor),
      boardAnalyzerThread(new QThread),
      boardInteractorThread(new QThread),
      katagoInteractorThread(new QThread),
      timer(new QTimer),
      timerThread(new QThread)
{
    boardAnalyzer->moveToThread(boardAnalyzerThread);
    boardInteractor->moveToThread(boardInteractorThread);
    katagoInteractor->moveToThread(katagoInteractorThread);
    connect(boardAnalyzerThread, &QThread::finished, boardAnalyzer, &BoardAnalyzer::deleteLater);
    connect(boardInteractorThread, &QThread::finished, boardInteractor, &BoardInteractor::deleteLater);
    connect(katagoInteractorThread, &QThread::finished, katagoInteractor, &KatagoInteractor::deleteLater);

    timer->setSingleShot(true);
    timer->setInterval(5000);

    timer->moveToThread(timerThread);
    connect(timerThread, &QThread::finished, timer, &QTimer::deleteLater);

    connect(this, &GameBoardHandler::startInit, this, &GameBoardHandler::init);

    connect(boardAnalyzerThread, &QThread::started, boardAnalyzer, &BoardAnalyzer::init);
    connect(boardInteractorThread, &QThread::started, boardInteractor, &BoardInteractor::init);
    connect(katagoInteractorThread, &QThread::started, katagoInteractor, &KatagoInteractor::init);

    connect(katagoInteractor, &KatagoInteractor::initFinished, this, &GameBoardHandler::startInitFinished);

    connect(this, &GameBoardHandler::startInitFinished, this, &GameBoardHandler::startGame);

    connect(this, &GameBoardHandler::startGame, boardInteractor, &BoardInteractor::startGame);

    connect(boardInteractor, &BoardInteractor::startGameFinished, boardAnalyzer, &BoardAnalyzer::waitForGameStarting);

    connect(boardAnalyzer, &BoardAnalyzer::gameStarted, this, &GameBoardHandler::checkMyStoneColorDelay);

    connect(boardAnalyzer, &BoardAnalyzer::myStoneColorUpdate, katagoInteractor, &KatagoInteractor::setMyStoneColor);
    connect(this, &GameBoardHandler::toPlay, boardInteractor, QOverload<const QPoint &>::of(&BoardInteractor::moveStone));
    connect(this, &GameBoardHandler::toStartAnalyzeIndefinitely, boardAnalyzer, &BoardAnalyzer::analyzeIndefinitely);
    connect(this, &GameBoardHandler::toStartCheckMyStoneColor, boardAnalyzer, &BoardAnalyzer::checkMyStoneColor);
    connect(boardAnalyzer, &BoardAnalyzer::myStoneColorUpdate, this, &GameBoardHandler::gameStartedHandle);

    connect(boardAnalyzer, &BoardAnalyzer::analyzeIndefinitelyData, this, &GameBoardHandler::onStoneMoved);
    connect(boardAnalyzer, &BoardAnalyzer::lastMoveStone, katagoInteractor, &KatagoInteractor::move);

    connect(this, &GameBoardHandler::toStartTime, timer, QOverload<>::of(&QTimer::start));
    connect(timer, &QTimer::timeout, katagoInteractor, &KatagoInteractor::getBestMove);
    connect(timer, &QTimer::timeout, boardAnalyzer, &BoardAnalyzer::stop);
    connect(timer, &QTimer::timeout, timer, []
            { qDebug() << QStringLiteral("time out"); });
    connect(boardInteractor, &BoardInteractor::moveFinished, boardAnalyzer, &BoardAnalyzer::analyzeIndefinitely);
    connect(katagoInteractor, &KatagoInteractor::bestMoveChanged, this, &GameBoardHandler::bestPointUpdate);
    connect(katagoInteractor, &KatagoInteractor::bestMove, boardInteractor, QOverload<const StoneData &>::of(&BoardInteractor::moveStone));

    connect(this, &GameBoardHandler::toAcceptRequest, boardInteractor, &BoardInteractor::acceptRequest);
    connect(this, &GameBoardHandler::toRejectRequest, boardInteractor, &BoardInteractor::rejectRequest);

    connect(this, &GameBoardHandler::gameOver, katagoInteractor, &KatagoInteractor::stopAnalyze);
    connect(this, &GameBoardHandler::gameOver, boardAnalyzer, &BoardAnalyzer::stop);
}

void GameBoardHandler::start()
{
    emit startInit();
}

void GameBoardHandler::init()
{
    boardAnalyzerThread->start();
    boardInteractorThread->start();
    katagoInteractorThread->start();
    timerThread->start();
}

void GameBoardHandler::gameStartedHandle(StoneData::StoneColor myStoneColor)
{
    qDebug() << Q_FUNC_INFO << myStoneColor;
    if (myStoneColor == StoneData::StoneColor::Black)
    {
        emit toPlay(QPoint(3, 3));
        return;
    }
    else if (myStoneColor == StoneData::StoneColor::White)
    {
        analyzeIndefinitelyDelay();
    }
    else if (myStoneColor == StoneData::StoneColor::None)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("myStoneColor is None");
        return;
    }
}

void GameBoardHandler::analyzeIndefinitelyDelay()
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(500, this, &GameBoardHandler::toStartAnalyzeIndefinitely);
}

void GameBoardHandler::onStoneMoved(const BoardData &boardData)
{
    qDebug() << Q_FUNC_INFO;
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

    if (boardData.getNeedMove())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("timer starting");
        emit toStartTime();
    }
}

void GameBoardHandler::checkMyStoneColorDelay()
{
    qDebug() << Q_FUNC_INFO;
    QTimer::singleShot(3000, this, &GameBoardHandler::toStartCheckMyStoneColor);
}
