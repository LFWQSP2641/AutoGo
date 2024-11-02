#include "AdbHandle.h"

#include <QDebug>
#include <QEventLoop>
#include <QProcess>
#include <QTimer>

AdbHandle *AdbHandle::m_instance = nullptr;

AdbHandle *AdbHandle::instance()
{
    if (!m_instance)
    {
        m_instance = new AdbHandle{};
    }
    return m_instance;
}

bool AdbHandle::isAdbConnected() const
{
    return m_isAdbConnected;
}

bool AdbHandle::init()
{
    if (m_isAdbConnected)
    {
        return true;
    }
    QEventLoop eventLoop;
    m_adbProcess->setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(m_adbProcess, &QProcess::finished, &eventLoop, &QEventLoop::quit);
    m_adbProcess->start(m_adbPath,
                        QStringList{ QStringLiteral("connect"),
                                     m_adbSerial });
    eventLoop.exec();
    QByteArray result(m_adbProcess->readAllStandardOutput());
    if (result.isEmpty())
        return false;
    for (const auto &i : QByteArrayList{ QByteArrayLiteral("cannot"), QByteArrayLiteral("failed") })
    {
        if (result.contains(i))
            return false;
    }
    return true;
}

QByteArray AdbHandle::executeAdbCommand(const QStringList &args)
{
    QEventLoop eventLoop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit); // 超时退出
    QObject::connect(m_adbProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &eventLoop, &QEventLoop::quit);

    m_adbProcess->start(m_adbPath, QStringList{ QStringLiteral("-s"), m_adbSerial } << args);

    // 检查进程是否启动成功
    if (!m_adbProcess->waitForStarted())
    {
        qWarning() << "ADB Process failed to start";
        return QByteArray();
    }

    timer.start(5000); // 设置超时，例如5秒
    eventLoop.exec();

    // 检查是否因为超时而退出
    if (timer.isActive())
    {
        timer.stop();
        // 判断进程是否正常退出
        if (m_adbProcess->exitStatus() == QProcess::NormalExit && m_adbProcess->error() == QProcess::UnknownError)
        {
            return m_adbProcess->readAllStandardOutput();
        }
        else
        {
            qWarning() << "ADB Process finished with error:" << m_adbProcess->errorString();
            return QByteArray();
        }
    }
    else
    {
        // 超时情况处理
        qWarning() << "ADB command timed out";
        m_adbProcess->kill();
        return QByteArray();
    }
}

AdbHandle::AdbHandle()
    : m_adbProcess{ new QProcess },
      m_isAdbConnected{ false }
{
}

AdbHandle::~AdbHandle()
{
    m_adbProcess->deleteLater();
}
