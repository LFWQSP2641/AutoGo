#ifndef STONEDATA_H
#define STONEDATA_H

#include <QObject>
#include <QPoint>

class StoneData
{
    Q_GADGET

public:
    StoneData();

    bool operator==(const StoneData &other) const;
    bool operator!=(const StoneData &other) const;

    enum StoneColor
    {
        None = 0,
        Black,
        White
    };
    Q_ENUM(StoneColor)

    StoneData(StoneData::StoneColor stoneColor, QPoint stonePoint);

    QPoint getPoint() const;
    void setPoint(QPoint newPoint);

    StoneData::StoneColor getColor() const;
    void setColor(StoneData::StoneColor newColor);

protected:
    QPoint point;
    StoneData::StoneColor color;

private:
    Q_PROPERTY(QPoint point READ getPoint WRITE setPoint FINAL)
    Q_PROPERTY(StoneData::StoneColor color READ getColor WRITE setColor FINAL)
};

QDebug operator<<(QDebug debug, const StoneData &stoneData);

#endif // STONEDATA_H
