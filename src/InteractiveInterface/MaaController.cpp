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
    maaTouchProcess->setProcessChannelMode(QProcess::MergedChannels);
}

bool MaaController::init()
{
    QString maatouchPath;
    for (const auto &i :
         QStringList{ QDir::currentPath().append(QStringLiteral("/maatouch")),
                      Global::dataPath().append(QStringLiteral("/maatouch")) })
    {
        if (QFile(i).exists())
        {
            maatouchPath = i;
            break;
        }
    }
    if (maatouchPath.isEmpty())
    {
        maatouchPath = Global::tempPath().append(QStringLiteral("/maatouch"));
        QFile::copy(QStringLiteral(":/data/maatouch"), maatouchPath);
    }
    if (!adbConnect())
        return false;
    const auto result(m_adbHandle->executeAdbCommand(
        QStringList{ QStringLiteral("push"),
                     maatouchPath,
                     QStringLiteral("/data/local/tmp/maatouch") }));
    if (!result.contains(QByteArrayLiteral("1 file pushed")))
        return false;
    QEventLoop eventLoop;
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
    qDebug() << p;
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
