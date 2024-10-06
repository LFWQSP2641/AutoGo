#include "Util.h"

#include <QDebug>
#include <QFile>
#include <QPoint>
#include <QRandomGenerator>
#include <cmath>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
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

bool Util::isRegionEqual(const cv::Mat &image, const QString &templateImageName, const QPoint &topLeft)
{
    QFile file(QStringLiteral(":/data/TemplateImage/")
                   .append(templateImageName)
                   .append(QStringLiteral(".png")));
    if (!file.open(QIODevice::ReadOnly))
    {
        qFatal() << QStringLiteral("无法打开模板图片文件: ")
                 << file.fileName();
        return false;
    }
    cv::Mat mat;
    qint64 sz = file.size();
    std::vector<uchar> buf(sz);
    file.read((char *)buf.data(), sz);
    mat = imdecode(buf, cv::IMREAD_COLOR);
    file.close();
    return isRegionEqual(image, mat, topLeft);
}

bool Util::isRegionEqual(const cv::Mat &image, const cv::Mat &templateImage, const QPoint &topLeft)
{
    // 将QPoint转换为cv::Point
    cv::Point topLeftCV(topLeft.x(), topLeft.y());

    // 使用templateImage的尺寸来创建区域
    cv::Rect region(topLeftCV.x, topLeftCV.y, templateImage.cols, templateImage.rows);

    // 确保区域在图像范围内
    if (region.x >= 0 && region.y >= 0 &&
        region.x + region.width <= image.cols &&
        region.y + region.height <= image.rows)
    {
        // 截取区域
        cv::Mat imageRegion = image(region);

        // 将图像和模板都转换为灰度图
        cv::Mat grayImageRegion, grayTemplateImage;
        cv::cvtColor(imageRegion, grayImageRegion, cv::COLOR_BGR2GRAY);
        cv::cvtColor(templateImage, grayTemplateImage, cv::COLOR_BGR2GRAY);

        // 进行逐像素比较
        return cv::countNonZero(grayImageRegion != grayTemplateImage) == 0;
    }
    return false;
}
