#include "KatagoInteractor.h"

#include "Settings.h"
#include "Util.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>

KatagoInteractor::KatagoInteractor(QObject *parent)
    : QObject{ parent },
      m_timeMode(KatagoInteractor::TimeMode::Short),
      katagoProcess(new QProcess(this)),
      emitBestMoveTimer(new QTimer(this)),
      chectKatagoOutputTimer(new QTimer(this))
{
    emitBestMoveTimer->setSingleShot(true);
    chectKatagoOutputTimer->setSingleShot(true);
    connect(emitBestMoveTimer, &QTimer::timeout, this, &KatagoInteractor::emitBestMove);
    connect(chectKatagoOutputTimer, &QTimer::timeout, this, &KatagoInteractor::checkKatagoOutput);

    connect(katagoProcess, &QProcess::readyReadStandardError, this, &KatagoInteractor::outputError);
}

void KatagoInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    m_boardData = BoardData();
}

void KatagoInteractor::startTimer()
{
    const auto moveSize(m_boardData.stoneCount());
    if (moveSize < 10)
    {
        emitBestMoveTimer->start(Util::generateTanhRandom(1000, 3000));
        return;
    }
    switch (m_timeMode)
    {
    case KatagoInteractor::Short:
        emitBestMoveTimer->start(Util::generateTanhRandom(5000, 15000));
        break;
    case KatagoInteractor::Medium:
        emitBestMoveTimer->start(Util::generateTanhRandom(10000, 30000));
        break;
    case KatagoInteractor::Long:
        emitBestMoveTimer->start(Util::generateTanhRandom(20000, 60000));
        break;
    default:
        break;
    }
}

void KatagoInteractor::quit()
{
    katagoProcess->close();
}

void KatagoInteractor::setTimeModeFromInt(int newTimeMode)
{
    setTimeMode(KatagoInteractor::TimeMode(newTimeMode));
}

qint64 KatagoInteractor::getMinMoveInterval() const
{
    return minMoveInterval;
}

void KatagoInteractor::setMinMoveInterval(qint64 newMinMoveInterval)
{
    if (minMoveInterval == newMinMoveInterval)
        return;
    minMoveInterval = newMinMoveInterval;
    emit minMoveIntervalChanged();
}

int KatagoInteractor::getReportIntervalMS() const
{
    return reportIntervalMS;
}

void KatagoInteractor::setReportIntervalMS(int newReportIntervalMS)
{
    if (reportIntervalMS == newReportIntervalMS)
        return;
    reportIntervalMS = newReportIntervalMS;
    emit reportIntervalMSChanged();
}

KatagoInteractor::TimeMode KatagoInteractor::timeMode() const
{
    return m_timeMode;
}

void KatagoInteractor::setTimeMode(KatagoInteractor::TimeMode newTimeMode)
{
    if (m_timeMode == newTimeMode)
        return;
    m_timeMode = newTimeMode;
    emit timeModeChanged();
}

void KatagoInteractor::init()
{
    disconnect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoInteractor::analyzeKatagoOutput);
    connect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoInteractor::analyzeKatagoInit);
    katagoProcess->setProcessChannelMode(QProcess::MergedChannels);
    katagoProcess->start(Settings::getSingletonSettings()->kataGoPath(),
                         getKataGoArgs());
}

QString KatagoInteractor::pointToGTP(const QPoint &point)
{
    const int x = point.x(); // 列数，对应字母
    const int y = point.y(); // 行数，需要反转

    // 将x转换为字母，跳过 'I'
    QChar columnLetter;
    if (x >= 8) // 当列数大于等于8时，跳过 'I'，从 'J' 开始
        columnLetter = QChar('A' + x + 1);
    else
        columnLetter = QChar('A' + x);

    // GTP 行数从 1 到 19，而 QPoint 的 y 从 0 到 18，所以 GTP 行号是 19 - y
    const int gtpRow = 19 - y;

    // 构建 GTP 格式的字符串，例如 "A1"
    const auto result(QString("%1%2").arg(columnLetter).arg(gtpRow));
    // qDebug() << Q_FUNC_INFO << point << result;
    return result;
}

QPoint KatagoInteractor::gptToPoint(const QString &gtpMove)
{
    QChar columnLetter = gtpMove[0];     // 列字母
    int gtpRow = gtpMove.mid(1).toInt(); // 行数字

    // 将字母转换为列数，跳过 'I'
    int x;
    if (columnLetter >= 'J') // 当列字母大于等于 'J' 时，跳过 'I'
        x = columnLetter.toUpper().unicode() - 'A' - 1;
    else
        x = columnLetter.toUpper().unicode() - 'A';

    // 将 GTP 行数转换为 QPoint 的 y 坐标，GTP 行从 1 到 19，QPoint 的 y 从 0 到 18，所以 y = 19 - 行数
    int y = 19 - gtpRow;

    const QPoint result(x, y);
    // qDebug() << Q_FUNC_INFO << gtpMove << result;
    return result;
}

QJsonArray KatagoInteractor::stoneDataListToJsonArray(const QList<StoneData> &stoneDataList)
{
    QJsonArray stonesMoveJsonArray;
    for (const auto &i : stoneDataList)
    {
        QJsonArray stoneJsonArray;
        switch (i.getColor())
        {
        case StoneData::StoneColor::Black:
            stoneJsonArray.append(QStringLiteral("B"));
            break;
        case StoneData::StoneColor::White:
            stoneJsonArray.append(QStringLiteral("W"));
            break;
        default:
            qWarning() << QStringLiteral("stone's color is none");
            break;
        }
        stoneJsonArray.append(pointToGTP(i.getPoint()));
        stonesMoveJsonArray.append(stoneJsonArray);
    }
    return stonesMoveJsonArray;
}

void KatagoInteractor::outputError()
{
    const auto error = katagoProcess->readAllStandardError();
    qDebug() << Q_FUNC_INFO << QStringLiteral("katago error:") << error;
    emit errorOccurred(error);
}

void KatagoInteractor::startCheckKatagoOutputTimer()
{
    switch (m_timeMode)
    {
    case KatagoInteractor::Short:
        chectKatagoOutputTimer->start(1000 * 25);
        break;
    case KatagoInteractor::Medium:
        chectKatagoOutputTimer->start(1000 * 35);
        break;
    case KatagoInteractor::Long:
        chectKatagoOutputTimer->start(1000 * 65);
        break;
    }
}

void KatagoInteractor::checkKatagoOutput()
{
    if (m_bestMove.getColor() != StoneData::StoneColor::None)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("unreachable");
        return;
    }
    qDebug() << Q_FUNC_INFO << QStringLiteral("checkKatagoOutput");
    emit logMessage(QStringLiteral("checkKatagoOutput"));
    clearCache();
    move(m_boardData);
}

void KatagoInteractor::emitBestMove()
{
    if (QDateTime::currentMSecsSinceEpoch() - lastMoveTime < minMoveInterval)
    {
        if (m_bestMove.getColor() == StoneData::StoneColor::None)
            qWarning() << QStringLiteral("bestMove is none");
        emitBestMoveTimer->start(100);
        return;
    }
    emit bestMove(m_bestMove);
    m_bestMove = StoneData();
}
