#ifndef UTIL_H
#define UTIL_H

#include <QObject>

namespace cv
{
class Mat;
} // namespace cv

namespace Util
{
Q_NAMESPACE
extern int generateTruncatedNormal(int minVal, int maxVal);
extern int generateTanhRandom(int minValue, int maxValue);
extern bool isRegionEqual(const cv::Mat &image, const QString &templateImageName, const QPoint &topLeft);
extern bool isRegionEqual(const cv::Mat &image, const cv::Mat &templateImage, const QPoint &topLeft);
} // namespace Util

#endif // UTIL_H
