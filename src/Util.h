#ifndef UTIL_H
#define UTIL_H

#include <QObject>

namespace Util
{
Q_NAMESPACE
extern int generateTruncatedNormal(int minVal, int maxVal);
extern int generateTanhRandom(int minValue, int maxValue);
extern bool isRegionEqual(const QImage &image, const QString &templateImageName, const QPoint &topLeft);
extern bool isRegionEqual(const QImage &image, const QImage &templateImage, const QPoint &topLeft);
} // namespace Util

#endif // UTIL_H
