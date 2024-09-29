#ifndef BOARDDATA_H
#define BOARDDATA_H

#include "StoneData.h"

#include <QObject>
#include <QPoint>

class BoardAnalyzer;

class BoardData
{
    Q_GADGET
    friend BoardAnalyzer;

public:
    BoardData();

    bool operator==(const BoardData &other) const;

    QList<QList<int>> getBoardDataArray() const;

    bool getRequestCounting() const;

    bool getRequestDraw() const;

    bool getRequestUndo() const;

    bool getGameOver() const;

    bool getUnknownUnexpected() const;

    bool getNeedMove() const;

    StoneData::StoneColor getMyStoneColor() const;

    StoneData getLastMoveStone() const;

    bool hasUnexpected() const;

    bool getIsMoving() const;

    QString getUuid() const;

    QList<StoneData> getInitialStonesArray() const;

    QList<StoneData> getMoveStonesArray() const;

protected:
    QList<QList<int>> boardDataArray;
    bool requestCounting;
    bool requestDraw;
    bool requestUndo;
    bool gameOver;
    bool unknownUnexpected;

    bool needMove;

    StoneData::StoneColor myStoneColor;

    StoneData lastMoveStone;

    bool isMoving;

    QString uuid;
    QList<StoneData> initialStonesArray;
    QList<StoneData> moveStonesArray;

private:
    Q_PROPERTY(QList<QList<int>> boardDataArray READ getBoardDataArray CONSTANT FINAL)
    Q_PROPERTY(bool requestCounting READ getRequestCounting CONSTANT FINAL)
    Q_PROPERTY(bool requestDraw READ getRequestDraw CONSTANT FINAL)
    Q_PROPERTY(bool requestUndo READ getRequestUndo CONSTANT FINAL)
    Q_PROPERTY(bool gameOver READ getGameOver CONSTANT FINAL)
    Q_PROPERTY(bool unknownUnexpected READ getUnknownUnexpected CONSTANT FINAL)
    Q_PROPERTY(bool needMove READ getNeedMove CONSTANT FINAL)
    Q_PROPERTY(StoneData::StoneColor myStoneColor READ getMyStoneColor CONSTANT FINAL)
    Q_PROPERTY(StoneData lastMoveStone READ getLastMoveStone CONSTANT FINAL)
    Q_PROPERTY(bool isMoving READ getIsMoving CONSTANT FINAL)
    Q_PROPERTY(QString uuid READ getUuid CONSTANT FINAL)
    Q_PROPERTY(QList<StoneData> initialStonesArray READ getInitialStonesArray CONSTANT FINAL)
    Q_PROPERTY(QList<StoneData> moveStonesArray READ getMoveStonesArray CONSTANT FINAL)
};

Q_DECLARE_METATYPE(BoardData)

#endif // BOARDDATA_H
