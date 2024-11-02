#pragma once

#include <QString>

namespace cv
{
class Mat;
} // namespace cv

// 全局变量以及不足以成一个类的成员都会放到这
namespace Global
{
Q_NAMESPACE
extern QString appConfigPath;
extern QString appDataPath;
extern QString appDebugPath;
extern QString appTempPath;

extern void initOnce();

extern QString configPath();
extern QString dataPath();
extern QString debugPath();
extern QString tempPath();

extern bool deleteDir(const QString &path);
extern qint64 getDirSize(const QString &filePath);
extern QString saveDebugImage(const cv::Mat &image);
}; // namespace Global
