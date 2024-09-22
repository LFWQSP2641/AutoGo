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

    QVector<QVector<int>> getBoardDataArray() const;

    bool getRequestCounting() const;

    bool getRequestDraw() const;

    bool getRequestUndo() const;

    bool getGameOver() const;

    bool getUnknownUnexpected() const;

    bool getNeedMove() const;

    StoneData::StoneColor getMyStoneColor() const;

    StoneData getLastMoveStone() const;

    bool hasUnexpected() const;

protected:
    QVector<QVector<int>> boardDataArray;
    bool requestCounting;
    bool requestDraw;
    bool requestUndo;
    bool gameOver;
    bool unknownUnexpected;

    bool needMove;

    StoneData::StoneColor myStoneColor;

    StoneData lastMoveStone;

private:
    Q_PROPERTY(QVector<QVector<int>> boardDataArray READ getBoardDataArray CONSTANT FINAL)
    Q_PROPERTY(bool requestCounting READ getRequestCounting CONSTANT FINAL)
    Q_PROPERTY(bool requestDraw READ getRequestDraw CONSTANT FINAL)
    Q_PROPERTY(bool requestUndo READ getRequestUndo CONSTANT FINAL)
    Q_PROPERTY(bool gameOver READ getGameOver CONSTANT FINAL)
    Q_PROPERTY(bool unknownUnexpected READ getUnknownUnexpected CONSTANT FINAL)
    Q_PROPERTY(bool needMove READ getNeedMove CONSTANT FINAL)
    Q_PROPERTY(StoneData::StoneColor myStoneColor READ getMyStoneColor CONSTANT FINAL)
    Q_PROPERTY(StoneData lastMoveStone READ getLastMoveStone CONSTANT FINAL)
};

Q_DECLARE_METATYPE(BoardData)

#endif // BOARDDATA_H
