#include "AdbScreencaptor.h"

#include "Settings.h"

#include <QEventLoop>
#include <QProcess>

AdbScreencaptor::AdbScreencaptor(QObject *parent)
    : Screencaptor{ parent },
      adbProcess{ new QProcess{ this } }
{
}

bool AdbScreencaptor::init()
{
    QEventLoop eventLoop;
    adbProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(adbProcess, &QProcess::finished, &eventLoop, &QEventLoop::quit);
    adbProcess->start(Settings::getSingletonSettings()->adbPath(),
                      QStringList{ QStringLiteral("connect"),
                                   Settings::getSingletonSettings()->adbSerial() });
    eventLoop.exec();
    QByteArray result(adbProcess->readAllStandardOutput());
    if (result.isEmpty())
        return false;
    for (const auto &i : QByteArrayList{ QByteArrayLiteral("cannot"), QByteArrayLiteral("failed") })
    {
        if (result.contains(i))
            return false;
    }
    return true;
}

std::optional<cv::Mat> AdbScreencaptor::screencap()
{
    QEventLoop eventLoop;
    connect(adbProcess, &QProcess::finished, &eventLoop, &QEventLoop::quit);
    adbProcess->start(Settings::getSingletonSettings()->adbPath(),
                      QStringList{ QStringLiteral("-s"),
                                   Settings::getSingletonSettings()->adbSerial(),
                                   QStringLiteral("exec-out"),
                                   QStringLiteral("screencap"),
                                   QStringLiteral("-p") });
    eventLoop.exec();
    QByteArray result(adbProcess->readAllStandardOutput());
    if (result.isEmpty())
        return std::nullopt;
    return std::optional<cv::Mat>{};
}
