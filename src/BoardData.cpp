#include "BoardData.h"

#include <QDateTime>

BoardData::BoardData()
    : BoardData{ QString::number(QDateTime::currentMSecsSinceEpoch()) }
{
}

BoardData::BoardData(QStringView uuid)
    : m_uuid(uuid),
      m_boardDataArray(QList<QList<int>>(19, QList<int>(19, 0))),
      m_lastMoveStone(StoneData()),
      m_myStoneColor(StoneData::StoneColor::None)
{
}

BoardData::BoardData(const BoardData &other)
    : m_uuid(QString::number(QDateTime::currentMSecsSinceEpoch())),
      m_boardDataArray(other.m_boardDataArray),
      m_lastMoveStone(other.m_lastMoveStone),
      m_initialStonesArray(other.m_initialStonesArray),
      m_moveStonesArray(other.m_moveStonesArray),
      m_myStoneColor(other.m_myStoneColor)
{
}

BoardData &BoardData::operator=(const BoardData &other)
{
    m_uuid = QString::number(QDateTime::currentMSecsSinceEpoch());
    m_boardDataArray = other.m_boardDataArray;
    m_lastMoveStone = other.m_lastMoveStone;
    m_initialStonesArray = other.m_initialStonesArray;
    m_moveStonesArray = other.m_moveStonesArray;
    m_myStoneColor = other.m_myStoneColor;
    return *this;
}

bool BoardData::operator==(const BoardData &other) const
{
    return m_boardDataArray == other.m_boardDataArray &&
           m_lastMoveStone == other.m_lastMoveStone;
}

bool BoardData::operator!=(const BoardData &other) const
{
    return !(*this == other);
}

QString BoardData::uuid() const
{
    return m_uuid;
}

QList<QList<int>> BoardData::boardDataArray() const
{
    return m_boardDataArray;
}

StoneData BoardData::lastMoveStone() const
{
    return m_lastMoveStone;
}

QList<StoneData> BoardData::initialStonesArray() const
{
    return m_initialStonesArray;
}

QList<StoneData> BoardData::moveStonesArray() const
{
    return m_moveStonesArray;
}

StoneData::StoneColor BoardData::myStoneColor() const
{
    return m_myStoneColor;
}

qsizetype BoardData::stoneCount() const
{
    return m_initialStonesArray.size() + m_moveStonesArray.size();
}
