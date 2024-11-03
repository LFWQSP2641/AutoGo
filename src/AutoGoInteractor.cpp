#include "AutoGoInteractor.h"

#include <QDebug>
#include <QThread>

AutoGoInteractor::AutoGoInteractor(QObject *parent)
    : QObject{ parent }, m_gameBoardHandler{ new GameBoardHandler }, m_gameBoardHandlerThread{ new QThread(this) }
{
    connect(this, &AutoGoInteractor::init, m_gameBoardHandler, &GameBoardHandler::init);
    connect(this, &AutoGoInteractor::startGame, m_gameBoardHandler, &GameBoardHandler::startGame);
    connect(this, &AutoGoInteractor::stopGame, m_gameBoardHandler, &GameBoardHandler::stopGame);
    connect(this, &AutoGoInteractor::continuousStartGame, m_gameBoardHandler, &GameBoardHandler::continuousStartGame);
    connect(this, &AutoGoInteractor::setTimeMode, m_gameBoardHandler, &GameBoardHandler::setTimeMode);

    connect(m_gameBoardHandler, &GameBoardHandler::boardDataArrayUpdate, this, &AutoGoInteractor::boardDataArrayUpdate);
    connect(m_gameBoardHandler, &GameBoardHandler::bestPointUpdate, this, &AutoGoInteractor::bestPointUpdate);
    connect(m_gameBoardHandler, &GameBoardHandler::initStarting, this, &AutoGoInteractor::initStarting);
    connect(m_gameBoardHandler, &GameBoardHandler::clearBestPoint, this, &AutoGoInteractor::clearBestPoint);
    connect(m_gameBoardHandler, &GameBoardHandler::gameStarting, this, &AutoGoInteractor::gameStarting);
    connect(m_gameBoardHandler, &GameBoardHandler::gameOver, this, &AutoGoInteractor::gameOver);
    connect(m_gameBoardHandler, &GameBoardHandler::startInitFinished, this, &AutoGoInteractor::startInitFinished);
    connect(m_gameBoardHandler, &GameBoardHandler::checkingAppNavigation, this, &AutoGoInteractor::checkingAppNavigation);

    // property
    connect(m_gameBoardHandler, &GameBoardHandler::taskChanged, this, &AutoGoInteractor::taskChanged);
    connect(m_gameBoardHandler, &GameBoardHandler::continuousPlayGameChanged, this, &AutoGoInteractor::continuousPlayGameChanged);

    m_gameBoardHandler->moveToThread(m_gameBoardHandlerThread);

    m_gameBoardHandlerThread->start();
}

AutoGoInteractor::~AutoGoInteractor()
{
    m_gameBoardHandler->deleteLater();
    m_gameBoardHandlerThread->quit();
    m_gameBoardHandlerThread->wait();
}

GameBoardHandler *AutoGoInteractor::gameBoardHandler() const
{
    return m_gameBoardHandler;
}
