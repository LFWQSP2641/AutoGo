#ifndef UTIL_H
#define UTIL_H

#include <QObject>

namespace Util
{
Q_NAMESPACE
extern int generateTruncatedNormal(int minVal, int maxVal);
extern int generateTanhRandom(int minValue, int maxValue);
} // namespace Util

#endif // UTIL_H
