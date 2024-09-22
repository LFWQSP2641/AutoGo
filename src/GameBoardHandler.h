#ifndef GAMEBOARDHANDLER_H
#define GAMEBOARDHANDLER_H

#include "BoardData.h"
#include "StoneData.h"

#include <QObject>
#include <QtQml/qqmlregistration.h>

class BoardAnalyzer;
class BoardInteractor;
class KatagoInteractor;

class GameBoardHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit GameBoardHandler(QObject *parent = nullptr);

public slots:
    void start();

protected:
    BoardAnalyzer *boardAnalyzer;
    BoardInteractor *boardInteractor;
    KatagoInteractor *katagoInteractor;

    QThread *boardAnalyzerThread;
    QThread *boardInteractorThread;
    QThread *katagoInteractorThread;

    QTimer *timer;
    QThread *timerThread;

protected slots:
    void init();

    void gameStartedHandle(StoneData::StoneColor myStoneColor);
    void analyzeIndefinitelyDelay();
    void onStoneMoved(const BoardData &boardData);
    void checkMyStoneColorDelay();

signals:
    void boardDataArrayUpdate(const QVector<QVector<int>> boardDataArray);
    void bestPointUpdate(const StoneData &stoneData);

    void startInit();
    void startGame();

    void gameOver();

    void startInitFinished(bool success);

    // 所有to字辈的用于多线程传递
    void toPlay(const QPoint &stonePoint);
    void toStartAnalyzeIndefinitely();
    void toStartCheckMyStoneColor();
    void toStartTime();

    void toAcceptRequest();
    void toRejectRequest();
};

#endif // GAMEBOARDHANDLER_H
