#include "Util.h"

#include <QRandomGenerator>
#include <cmath>
#include <random>

int Util::generateTruncatedNormal(int minVal, int maxVal)
{
    const double averageVal = static_cast<double>(minVal + maxVal) / 2.0;
    const double stddev = static_cast<double>(maxVal - minVal) / 4.0;
    std::normal_distribution<double> dist(averageVal, stddev);
    QRandomGenerator *generator = QRandomGenerator::global();
    double value;
    do
    {
        value = dist(*generator);
    } while (value < minVal || value > maxVal);

    return qRound(value); // 使用四舍五入
}

int Util::generateTanhRandom(int minValue, int maxValue)
{
    QRandomGenerator *generator = QRandomGenerator::global();

    // 使用标准正态分布生成随机数
    std::normal_distribution<double> dist(0.0, 1.0);

    // 使用双曲正切函数将随机数限制在[-1, 1]范围内
    double normalRandom = dist(*generator);
    double boundedRandom = tanh(normalRandom);

    // 将随机数缩放到[minValue, maxValue]范围内
    double scaledRandom = (boundedRandom + 1.0) / 2.0 * (maxValue - minValue) + minValue;

    return qRound(scaledRandom); // 使用四舍五入
}
