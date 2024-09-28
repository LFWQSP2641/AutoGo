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
    void startGame();
    void continuousStartGame();

protected:
    BoardAnalyzer *boardAnalyzer;
    BoardInteractor *boardInteractor;
    KatagoInteractor *katagoInteractor;

    QThread *boardAnalyzerThread;
    QThread *boardInteractorThread;
    QThread *katagoInteractorThread;

    // QTimer *timer;
    // QThread *timerThread;

    bool inited;

protected slots:
    void init();

    void gameStartedHandle(StoneData::StoneColor myStoneColor);
    void onStoneMoved(const BoardData &boardData);
    void checkMyStoneColorDelay();

    void onInitFinished();

signals:
    void boardDataArrayUpdate(const QList<QList<int>> boardDataArray);
    void bestPointUpdate(const StoneData &stoneData);

    void startInit();

    void clearBestPoint();

    void gameStarted();
    void gameOver();

    void startInitFinished(bool success);

    void checkingAppNavigation();

    // 所有to字辈的用于多线程传递
    void toStartGame();
    void toPlay(const QPoint &stonePoint);
    void toStartAnalyzeIndefinitely();
    void toStartCheckMyStoneColor();
    void toStartTimer(int msec);

    void toAcceptRequest();
    void toRejectRequest();

    void toReset();
};

#endif // GAMEBOARDHANDLER_H
