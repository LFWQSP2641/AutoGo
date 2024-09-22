#include "Controller.h"

#include <QRandomGenerator>
#include <QRect>
#include <random>

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
    int x = normDistributionGenerator(xMin, xMax);
    int y = normDistributionGenerator(yMin, yMax);

    // 返回生成的QPoint
    return QPoint(x, y);
}

int Controller::normDistributionGenerator(int minVal, int maxVal)
{
    const double averageVal(static_cast<double>(minVal + maxVal) / double(2.0));
    const double stddev(static_cast<double>(maxVal - minVal) / double(4.0));
    std::normal_distribution dist(averageVal, stddev);
    QRandomGenerator *generator = QRandomGenerator::global();
    double value;
    do
    {
        value = dist(*generator);               // 使用QRandomGenerator
    } while (value < minVal || value > maxVal); // 通过限制生成的值

    return value;
}
