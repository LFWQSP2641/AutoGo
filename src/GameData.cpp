#include "GameData.h"

#include <QDateTime>

GameData::GameData()
    : GameData{ QString::number(QDateTime::currentMSecsSinceEpoch()) }
{
}

GameData::GameData(QStringView uuid)
    : m_uuid(uuid),
      m_boardData(BoardData(uuid)),
      m_appNavigation(AppNavigation::unknownPage),
      m_myStoneColor(StoneData::StoneColor::None),
      m_needMove(false)
{
}

GameData::GameData(const GameData &other)
    : m_uuid(other.m_uuid),
      m_boardData(other.m_boardData),
      m_appNavigation(other.m_appNavigation),
      m_myStoneColor(other.m_myStoneColor),
      m_needMove(other.m_needMove)
{
}

GameData &GameData::operator=(const GameData &other)
{
    m_uuid = other.m_uuid;
    m_boardData = other.m_boardData;
    m_appNavigation = other.m_appNavigation;
    m_myStoneColor = other.m_myStoneColor;
    m_needMove = other.m_needMove;
    return *this;
}

bool GameData::operator==(const GameData &other) const
{
    return m_boardData == other.m_boardData &&
           m_appNavigation == other.m_appNavigation &&
           m_myStoneColor == other.m_myStoneColor &&
           m_needMove == other.m_needMove;
}

bool GameData::operator!=(const GameData &other) const
{
    return !(*this == other);
}

QString GameData::uuid() const
{
    return m_uuid;
}

BoardData GameData::boardData() const
{
    return m_boardData;
}

GameData::AppNavigation GameData::appNavigation() const
{
    return m_appNavigation;
}

StoneData::StoneColor GameData::myStoneColor() const
{
    return m_myStoneColor;
}

bool GameData::needMove() const
{
    return m_needMove;
}
