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
    void closeRequest();

    void matchGame();

    void closeGameOverDialog();

    void backToMain();

protected:
    MaaController *controller;

protected slots:
    void matchGame1();
    void matchGame2();
    void clickConfirmMove();

signals:
    void moveFinished();
};

#endif // BOARDINTERACTOR_H
