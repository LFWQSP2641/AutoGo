#ifndef BOARDINTERACTOR_H
#define BOARDINTERACTOR_H

#include "StoneData.h"

#include <QObject>

class MaaController;

class BoardInteractor : public QObject
{
    Q_OBJECT

public:
    explicit BoardInteractor(QObject *parent = nullptr);

public slots:
    void init();

    void moveStone(const StoneData &stoneData);
    void moveStone(const QPoint &stonePoint);

    void acceptRequest();
    void rejectRequest();

    void startGame();

protected:
    MaaController *controller;

protected slots:
    void startGame1();
    void startGame2();
    void clickConfirmMove();

signals:
    void startGameFinished();
    void moveFinished();
};

#endif // BOARDINTERACTOR_H
