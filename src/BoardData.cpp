#include "BoardData.h"

BoardData::BoardData()
    : boardDataArray(QVector<QVector<int>>(19, QVector<int>(19, 0))),
      requestCounting(false),
      requestDraw(false),
      requestUndo(false),
      gameOver(false),
      unknownUnexpected(false),
      needMove(false),
      myStoneColor(StoneData::StoneColor::None),
      lastMoveStone(StoneData())
{
}

bool BoardData::operator==(const BoardData &other) const
{
    return boardDataArray == other.boardDataArray &&
           requestCounting == other.requestCounting &&
           requestDraw == other.requestDraw &&
           requestUndo == other.requestUndo &&
           gameOver == other.gameOver &&
           needMove == other.needMove &&
           myStoneColor == other.myStoneColor &&
           lastMoveStone == other.lastMoveStone;
}

QVector<QVector<int>> BoardData::getBoardDataArray() const
{
    return boardDataArray;
}

bool BoardData::getRequestCounting() const
{
    return requestCounting;
}

bool BoardData::getRequestDraw() const
{
    return requestDraw;
}

bool BoardData::getRequestUndo() const
{
    return requestUndo;
}

bool BoardData::getGameOver() const
{
    return gameOver;
}

bool BoardData::getUnknownUnexpected() const
{
    return unknownUnexpected;
}

bool BoardData::getNeedMove() const
{
    return needMove;
}

StoneData::StoneColor BoardData::getMyStoneColor() const
{
    return myStoneColor;
}

StoneData BoardData::getLastMoveStone() const
{
    return lastMoveStone;
}

bool BoardData::hasUnexpected() const
{
    return requestCounting || requestDraw || requestUndo || gameOver ||
           unknownUnexpected;
}
