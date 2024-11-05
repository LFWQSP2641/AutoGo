#include "AdbScreencaptor.h"

#include <QEventLoop>
#include <QProcess>

AdbScreencaptor::AdbScreencaptor(QObject *parent)
    : Screencaptor{ parent }
{
}

bool AdbScreencaptor::init()
{
    return adbConnect();
}

std::optional<QImage> AdbScreencaptor::screencap()
{
    QByteArray result(m_adbHandle->executeAdbCommand(
        QStringList{ QStringLiteral("exec-out"), QStringLiteral("screencap"),
                     QStringLiteral("-p") }));
    if (result.isEmpty())
        return std::nullopt;
    return std::optional<QImage>{ QImage::fromData(result) };
}
