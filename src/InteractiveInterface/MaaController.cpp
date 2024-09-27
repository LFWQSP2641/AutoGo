#include "MaaController.h"

#include "src/Global.h"
#include "src/Settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QPoint>
#include <QPointer>
#include <QProcess>
#include <QThread>

MaaController::MaaController(QObject *parent)
    : Controller{ parent },
      maaTouchProcess(new QProcess(this))
{
}

bool MaaController::init()
{
    QString maatouchPath;
    for (const auto &i :
         QStringList{QDir::currentPath().append(QStringLiteral("/maatouch")),
                     Global::dataPath().append(QStringLiteral("/maatouch"))}) {
        if (QFile(i).exists()) {
            maatouchPath = i;
            break;
        }
    }
    if (maatouchPath.isEmpty())
        return false;
    QEventLoop eventLoop;
    maaTouchProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(maaTouchProcess, &QProcess::finished, &eventLoop, &QEventLoop::quit);
    maaTouchProcess->start(Settings::getSingletonSettings()->adbPath(),
                           QStringList{ QStringLiteral("connect"),
                                        Settings::getSingletonSettings()->adbSerial() });
    eventLoop.exec();
    QString result(maaTouchProcess->readAllStandardOutput());
    if (result.isEmpty())
        return false;
    for (const auto &i : QStringList{ QStringLiteral("cannot"), QStringLiteral("failed") })
    {
        if (result.contains(i))
            return false;
    }
    maaTouchProcess->start(Settings::getSingletonSettings()->adbPath(),
                           QStringList{ QStringLiteral("-s"),
                                        Settings::getSingletonSettings()->adbSerial(),
                                        QStringLiteral("push"),
                                        maatouchPath,
                                        QStringLiteral("/data/local/tmp/maatouch") });
    eventLoop.exec();
    result = maaTouchProcess->readAllStandardOutput();
    if (!result.contains(QStringLiteral("1 file pushed")))
        return false;
    connect(maaTouchProcess, &QProcess::started, &eventLoop, &QEventLoop::quit, Qt::SingleShotConnection);
    maaTouchProcess->start(
        Settings::getSingletonSettings()->adbPath(),
        QStringList{
            QStringLiteral("-s"), Settings::getSingletonSettings()->adbSerial(),
            QStringLiteral("shell"),
            QStringLiteral(
                "export CLASSPATH=/data/local/tmp/maatouch; app_process "
                "/data/local/tmp com.shxyke.MaaTouch.App") });
    if (maaTouchProcess->state() == QProcess::Starting)
        eventLoop.exec();
    return true;
}

bool MaaController::click(const QPoint &p)
{
    qDebug() << Q_FUNC_INFO << p;
    if (!maaTouchProcess->isOpen())
        return false;
    QByteArray bytes(QStringLiteral("d 0 %0 %1 1\r\n")
                         .arg(p.x())
                         .arg(p.y())
                         .append("u 0\r\n")
                         .append("c\r\n")
                         .toUtf8());
    maaTouchProcess->write(bytes);
    return true;
}
