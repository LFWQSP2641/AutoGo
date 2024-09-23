#include "Controller.h"

#include "src/Util.h"

#include <QRect>

Controller::Controller(QObject *parent)
    : QObject{ parent }
{
}

bool Controller::click(const QRect &rect)
{
    return click(normDistributionGenerator(rect));
}

QPoint Controller::normDistributionGenerator(const QRect &rect)
{
    // 获取 QRect 的 x 和 y 的最小值和最大值
    int xMin = rect.left();
    int xMax = rect.right();
    int yMin = rect.top();
    int yMax = rect.bottom();

    // 使用正态分布生成x和y坐标
    int x = Util::generateTanhRandom(xMin, xMax);
    int y = Util::generateTanhRandom(yMin, yMax);

    // 返回生成的QPoint
    return QPoint(x, y);
}
