#ifndef BOARDDATA_H
#define BOARDDATA_H

#include "StoneData.h"

#include <QObject>
#include <QPoint>

class GameAnalyzer;

class BoardData
{
    Q_GADGET
    friend GameAnalyzer;

public:
    BoardData();
    explicit BoardData(QStringView uuid);
    BoardData(const BoardData &other);
    BoardData &operator=(const BoardData &other);

    bool operator==(const BoardData &other) const;
    bool operator!=(const BoardData &other) const;

    QString uuid() const;
    QList<QList<int>> boardDataArray() const;
    StoneData lastMoveStone() const;
    QList<StoneData> initialStonesArray() const;
    QList<StoneData> moveStonesArray() const;

    StoneData::StoneColor myStoneColor() const;

    qsizetype stoneCount() const;

protected:
    QString m_uuid;
    QList<QList<int>> m_boardDataArray;
    StoneData m_lastMoveStone;
    QList<StoneData> m_initialStonesArray;
    QList<StoneData> m_moveStonesArray;

    // TODO 优化
    StoneData::StoneColor m_myStoneColor;

private:
    Q_PROPERTY(QString uuid READ uuid CONSTANT FINAL)
    Q_PROPERTY(QList<QList<int>> boardDataArray READ boardDataArray CONSTANT FINAL)
    Q_PROPERTY(StoneData lastMoveStone READ lastMoveStone CONSTANT FINAL)
    Q_PROPERTY(QList<StoneData> initialStonesArray READ initialStonesArray CONSTANT FINAL)
    Q_PROPERTY(QList<StoneData> moveStonesArray READ moveStonesArray CONSTANT FINAL)
    Q_PROPERTY(StoneData::StoneColor myStoneColor READ myStoneColor CONSTANT FINAL)
};

Q_DECLARE_METATYPE(BoardData)

#endif // BOARDDATA_H
