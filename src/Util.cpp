#include "Util.h"

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPoint>
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

bool Util::isRegionEqual(const QImage &image, const QString &templateImageName, const QPoint &topLeft)
{
    const QString templateImagePath = QStringLiteral(":/data/TemplateImage/")
                                          .append(templateImageName)
                                          .append(QStringLiteral(".png"));
    QImage mat(templateImagePath);
    if (mat.isNull())
    {
        qFatal() << QStringLiteral("无法加载模板图片: ")
                 << templateImagePath;
        return false;
    }
    return isRegionEqual(image, mat, topLeft);
}

bool Util::isRegionEqual(const QImage &image, const QImage &templateImage, const QPoint &topLeft)
{
    QRect region(topLeft, templateImage.size());

    // 确保区域在图像范围内
    if (region.x() >= 0 && region.y() >= 0 &&
        region.x() + region.width() <= image.width() &&
        region.y() + region.height() <= image.height())
    {
        quint64 sumDiff(0);
        for (int i = 0; i < templateImage.width(); ++i)
        {
            for (int j = 0; j < templateImage.height(); ++j)
            {
                QColor imageColor = image.pixelColor(i + topLeft.x(), j + topLeft.y());
                QColor templateColor = templateImage.pixelColor(i, j);
                sumDiff += qAbs(imageColor.red() - templateColor.red());
                sumDiff += qAbs(imageColor.green() - templateColor.green());
                sumDiff += qAbs(imageColor.blue() - templateColor.blue());
            }
        }

        // 计算差值的平均值
        double meanDiff = static_cast<double>(sumDiff) / (templateImage.width() * templateImage.height() * 3);

        return meanDiff < 1.0;
    }
    return false;
}
