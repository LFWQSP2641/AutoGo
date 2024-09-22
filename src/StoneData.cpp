#include "StoneData.h"

#include <QDebug>

StoneData::StoneData()
    : StoneData(StoneData::StoneColor::None, QPoint(-1, -1))
{
}

bool StoneData::operator==(const StoneData &other) const
{
    return point == other.point &&
           color == other.color;
}

bool StoneData::operator!=(const StoneData &other) const
{
    return !(*this == other);
}

StoneData::StoneData(StoneColor stoneColor, QPoint stonePoint)
    : point(stonePoint),
      color(stoneColor)
{
}

QPoint StoneData::getPoint() const
{
    return point;
}

void StoneData::setPoint(QPoint newPoint)
{
    if (point == newPoint)
        return;
    point = newPoint;
}

StoneData::StoneColor StoneData::getColor() const
{
    return color;
}

void StoneData::setColor(StoneData::StoneColor newColor)
{
    if (color == newColor)
        return;
    color = newColor;
}

QDebug operator<<(QDebug debug, const StoneData &stoneData)
{
    debug << stoneData.getColor() << stoneData.getPoint();
    return debug;
}
