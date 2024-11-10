#include "GameBoardHandler.h"

#include "GameAnalyzer.h"
#include "GameInteractor.h"
#include "InteractiveInterface/AdbScreencaptor.h"
#include "InteractiveInterface/MaaController.h"
#include "InteractiveInterface/MumuScreencaptor.h"
#include "KatagoAnalysisInteractor.h"
#include "KatagoGTPInteractor.h"
#include "Settings.h"

#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QTimer>

GameBoardHandler::GameBoardHandler(QObject *parent)
    : QObject{ parent },
      m_gameAnalyzer(nullptr),
      m_gameInteractor(nullptr),
      m_katagoInteractor(nullptr),
      m_gameAnalyzerThread(nullptr),
      m_katagoInteractorThread(nullptr),
      m_inited(false),
      m_state(State::Stopped),
      m_task(Task::NoTask),
      m_checkTimer(nullptr),
      gameCount(0),
      gameStarted(false),
      continuousPlayGame(false),
      m_timeMode(0),
      m_pauseReception(false)
{
}

GameBoardHandler::~GameBoardHandler()
{
    if (!m_inited)
        return;
    m_gameAnalyzer->stopAnalyze();
    m_katagoInteractor->quit();
    m_gameAnalyzerThread->quit();
    m_katagoInteractorThread->quit();
    m_gameAnalyzerThread->wait();
    m_katagoInteractorThread->wait();
}

bool GameBoardHandler::inited() const
{
    return m_inited;
}

GameBoardHandler::State GameBoardHandler::state() const
{
    return m_state;
}

GameBoardHandler::Task GameBoardHandler::task() const
{
    return m_task;
}

void GameBoardHandler::setTask(GameBoardHandler::Task newTask)
{
    if (m_task == newTask)
        return;
    m_task = newTask;
    emit taskChanged();
}

int GameBoardHandler::getGameCount() const
{
    return gameCount;
}

bool GameBoardHandler::getGameStarted() const
{
    return gameStarted;
}

bool GameBoardHandler::getContinuousPlayGame() const
{
    return continuousPlayGame;
}

void GameBoardHandler::setContinuousPlayGame(bool newContinuousPlayGame)
{
    if (continuousPlayGame == newContinuousPlayGame)
        return;
    continuousPlayGame = newContinuousPlayGame;
    emit continuousPlayGameChanged();
}

void GameBoardHandler::init()
{
    if (m_inited)
        return;

    m_checkTimer = new QTimer(this);
    m_checkTimer->setInterval(1000);
    m_checkTimer->setSingleShot(true);

    m_gameAnalyzerThread = new QThread(this);
    m_katagoInteractorThread = new QThread(this);

    m_gameAnalyzer = new GameAnalyzer;
    m_gameInteractor = new GameInteractor(this);

    if (Settings::getSingletonSettings()->kataGoMode() == QStringLiteral("Analysis"))
    {
        qDebug() << QStringLiteral("Analysis");
        m_katagoInteractor = new KatagoAnalysisInteractor;
    }
    else if (Settings::getSingletonSettings()->kataGoMode() == QStringLiteral("GTP"))
    {
        qDebug() << QStringLiteral("GTP");
        m_katagoInteractor = new KatagoGTPInteractor;
    }
    else
        qFatal() << QStringLiteral("kataGoMode is invalid");

    m_gameInteractor->setController(new MaaController(m_gameInteractor));

    Screencaptor *screencaptor;
    if (Settings::getSingletonSettings()->mumuEnable())
    {
        screencaptor = new MumuScreencaptor(m_gameAnalyzer);
    }
    else
    {
        screencaptor = new AdbScreencaptor(m_gameAnalyzer);
    }
    m_gameAnalyzer->setScreencaptor(screencaptor);

    m_gameInteractor->setTimeModeFromInt(m_timeMode);
    m_katagoInteractor->setTimeModeFromInt(m_timeMode);

    m_gameAnalyzer->moveToThread(m_gameAnalyzerThread);
    m_katagoInteractor->moveToThread(m_katagoInteractorThread);

    connectSignals();

    m_gameAnalyzerThread->start();
    m_katagoInteractorThread->start();

    emit toStartInit();
    emit initStarting();
}

void GameBoardHandler::startGame()
{
    m_task = Task::StartGame;
    startTask();
}

void GameBoardHandler::stopGame()
{
    m_task = Task::StopGame;
    startTask();
}

void GameBoardHandler::continuousStartGame()
{
    m_task = Task::StartGame;
    QTimer::singleShot(3000, this, &GameBoardHandler::startTask);
}

void GameBoardHandler::setTimeMode(int timeMode)
{
    qDebug() << timeMode;
    m_timeMode = timeMode;
    emit toSetTimeMode(m_timeMode);
}

void GameBoardHandler::connectSignals()
{
    connect(m_checkTimer, &QTimer::timeout, this, &GameBoardHandler::checkStoneMove);

    connect(this, &GameBoardHandler::toStartInit, m_gameAnalyzer, &GameAnalyzer::init);
    connect(this, &GameBoardHandler::toStartInit, m_katagoInteractor, &KatagoInteractor::init);
    connect(this, &GameBoardHandler::toStartInit, m_gameInteractor, &GameInteractor::init);

    connect(m_katagoInteractor, &KatagoInteractor::initFinished, this, &GameBoardHandler::onInitFinished);
    connect(m_gameAnalyzerThread, &QThread::finished, m_gameAnalyzer, &QObject::deleteLater);
    connect(m_katagoInteractorThread, &QThread::finished, m_katagoInteractor, &QObject::deleteLater);

    connect(m_gameAnalyzer, &GameAnalyzer::analysisDataUpdated, this, &GameBoardHandler::onGameAnalyzerDataUpdated);
    connect(m_katagoInteractor, &KatagoInteractor::bestMove, this, &GameBoardHandler::onKatagoBestMove);

    connect(this, &GameBoardHandler::toSetTimeMode, m_gameInteractor, &GameInteractor::setTimeModeFromInt);
    connect(this, &GameBoardHandler::toSetTimeMode, m_katagoInteractor, &KatagoInteractor::setTimeModeFromInt);

    connect(this, &GameBoardHandler::toAnalyzeGameBoard, m_gameAnalyzer, &GameAnalyzer::analyzeIndefinitely);
    connect(this, &GameBoardHandler::toStopGame, m_katagoInteractor, &KatagoInteractor::stopAnalyze);
    connect(this, &GameBoardHandler::toStopGame, m_gameAnalyzer, &GameAnalyzer::stopAnalyze);

    connect(this, &GameBoardHandler::toKatagoMove, m_katagoInteractor, &KatagoInteractor::move);
    connect(this, &GameBoardHandler::toPauseAnalyze, m_gameAnalyzer, &GameAnalyzer::pause);

    connect(this, &GameBoardHandler::toAnalyzeReset, m_gameAnalyzer, &GameAnalyzer::reset);

    connect(m_katagoInteractor, &KatagoInteractor::logMessage, this, &GameBoardHandler::logMessage);
    connect(m_katagoInteractor, &KatagoInteractor::errorOccurred, this, &GameBoardHandler::errorOccurred);
}

void GameBoardHandler::startTask()
{
    qDebug() << Q_FUNC_INFO << m_task;
    if (!m_inited)
    {
        qWarning() << QStringLiteral("GameBoardHandler has not been inited");
        return;
    }
    m_pauseReception = false;
    switch (m_task)
    {
    case StopGame:
        emit toStopGame();
        break;
    case StartGame:
        gameStarted = false;
        emit toAnalyzeGameBoard();
        break;
    case NoTask:
    default:
        break;
    }
}

void GameBoardHandler::handleGamePage(GameData::AppNavigation appNavigation)
{
    switch (appNavigation)
    {
    case GameData::AppNavigation::playingPageWithMove:
        m_gameInteractor->clickConfirmMove();
        break;
    case GameData::AppNavigation::gameOverDialog:
        m_gameInteractor->closeGameOverDialog();
        break;
    case GameData::AppNavigation::levelUpDialog:
        m_gameInteractor->closeLevelUpDialog();
        break;
    case GameData::AppNavigation::mainPage:
        m_gameInteractor->openMatchDialog();
        break;
    case GameData::AppNavigation::durationChoiceDialog:
        m_gameInteractor->choiceGameMode();
        break;
    case GameData::AppNavigation::analysisPage:
    case GameData::AppNavigation::pageWithBack:
        m_gameInteractor->backToMain();
        break;
    case GameData::AppNavigation::requestCountingDialog:
    case GameData::AppNavigation::requestRematchDialog:
    case GameData::AppNavigation::requestIntelligentRefereeDialog:
        m_gameInteractor->acceptRequest();
        break;
    case GameData::AppNavigation::requestUndoDialog:
    case GameData::AppNavigation::requestDrawDialog:
    case GameData::AppNavigation::requestResumeBattleDialog:
    case GameData::AppNavigation::confirmDefeatDialog:
    case GameData::AppNavigation::otherDialog:
        m_gameInteractor->rejectRequest();
        break;
    case GameData::AppNavigation::cancelResumeBattleDialog:
    case GameData::AppNavigation::intelligentRefereeDialogFailed:
    case GameData::AppNavigation::ReconnectSuccessAndFinishDialog:
        m_gameInteractor->closeRequest();
        break;
    case GameData::AppNavigation::acceptCountingResultDialog:
        m_gameInteractor->acceptCountingResult();
        break;
    case GameData::AppNavigation::matchDialog:
        emit toPauseAnalyze(500);
        break;
    case GameData::AppNavigation::AIPKDialog:
        m_gameInteractor->closeAIPKDialog();
        break;
    case GameData::AppNavigation::tipDialog:
    default:
        break;
    }
}

void GameBoardHandler::checkStoneMove()
{
    if (m_state != State::Playing)
        return;
    if (lastMoveStone != lastBestMoveStone)
    {
        m_gameInteractor->moveStone(lastBestMoveStone);
    }
}

void GameBoardHandler::handleGameOpening(const GameData &gameData)
{
    const auto myStoneColor(gameData.myStoneColor());
    qDebug() << myStoneColor;
    switch (myStoneColor)
    {
    case StoneData::StoneColor::Black:
        qDebug() << QStringLiteral("Black");
        m_gameInteractor->moveStone(QPoint(3, 3));
        break;
    case StoneData::StoneColor::None:
        qWarning() << QStringLiteral("myStoneColor is None");
        break;
    default:
        break;
    }
}

void GameBoardHandler::pauseGameAnalyze(const unsigned long &duration)
{
    m_pauseReception = true;
    m_gameAnalyzer->stopAnalyze();
    QTimer::singleShot(duration, this, [this]
                       {
                           this->m_pauseReception = false;
                           emit this->toAnalyzeGameBoard(); });
}

void GameBoardHandler::onInitFinished(bool success)
{
    m_inited = success;
    emit startInitFinished(success);
}

void GameBoardHandler::onGameAnalyzerDataUpdated(const GameData &gameData)
{
    qDebug() << gameData.appNavigation();
    if (m_pauseReception)
        return;
    if (m_task != Task::StartGame)
        return;
    // 对局结束
    if (gameStarted && gameData.appNavigation() == GameData::AppNavigation::gameOverDialog)
    {
        gameStarted = false;
        gameCount++;
        emit gameOver();
        if (continuousPlayGame)
        {
            m_gameInteractor->closeGameOverDialog();
            // 等待界面反应
            pauseGameAnalyze(2000);
        }
        else
        {
            m_pauseReception = true;
            m_state = State::Stopped;
            emit toStopGame();
        }
        return;
    }
    handleGamePage(gameData.appNavigation());
    if (gameData.appNavigation() == GameData::AppNavigation::playingPage && m_state != State::WaitingCounterpartyAccept)
    {
        m_state = State::Playing;
        if (!gameStarted)
        {
            // 对局开始
            pauseGameAnalyze(2000);
            QTimer::singleShot(2000, this, [this, gameData]
                               { this->handleGameOpening(gameData); });
            gameStarted = true;
            emit gameStarting();
            return;
        }
        lastMoveStone = gameData.boardData().lastMoveStone();
        emit boardDataArrayUpdate(gameData.boardData().boardDataArray());

        if (gameData.needMove())
        {
            // 340 341 350 351
            if (gameData.boardData().stoneCount() / 2 == 170 || gameData.boardData().stoneCount() / 2 == 175)
            {
                // TODO 请求智能裁判后, 会有一个对话框, 需要处理
                m_state = State::WaitingCounterpartyAccept;
                m_gameInteractor->requestIntelligentReferee();
            }
            else
                emit toKatagoMove(gameData.boardData());
        }
    }
    else if (gameData.appNavigation() == GameData::AppNavigation::mainPage ||
             gameData.appNavigation() == GameData::AppNavigation::durationChoiceDialog ||
             gameData.appNavigation() == GameData::AppNavigation::matchDialog)
    {
        m_state = State::WaitingMatching;
    }
    else if (gameData.appNavigation() == GameData::AppNavigation::intelligentRefereeDialogFailed)
    {
        m_state = State::Playing;
    }
    else if (gameData.appNavigation() == GameData::AppNavigation::gameOverDialog)
    {
        m_state = State::Stopped;
    }
    else
    {
        m_state = State::NavigatingToMain;
    }
}

void GameBoardHandler::onKatagoBestMove(const StoneData &stoneData)
{
    if (lastBestMoveStone == stoneData)
        return;
    lastBestMoveStone = stoneData;
    emit bestPointUpdate(stoneData);
    m_gameInteractor->moveStone(stoneData);
    m_checkTimer->start();
    m_state = State::WaitingCounterpartyMove;
}
