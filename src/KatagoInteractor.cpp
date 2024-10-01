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
#include <QTimer>

KatagoInteractor::KatagoInteractor(QObject *parent)
    : QObject{ parent },
      m_timeMode(KatagoInteractor::TimeMode::Short),
      katagoProcess(new QProcess(this)),
      timer(new QTimer(this))
{
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &KatagoInteractor::emitBestMove);
}

void KatagoInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    m_boardData = BoardData();
}

void KatagoInteractor::startTimer()
{
    const auto moveSize(m_boardData.getInitialStonesArray().size() + m_boardData.getMoveStonesArray().size());
    if (moveSize < 10)
    {
        timer->start(Util::generateTanhRandom(1000, 3000));
        return;
    }
    switch (m_timeMode)
    {
    case KatagoInteractor::Short:
        timer->start(Util::generateTanhRandom(5000, 15000));
        break;
    case KatagoInteractor::Medium:
        timer->start(Util::generateTanhRandom(10000, 30000));
        break;
    case KatagoInteractor::Long:
        timer->start(Util::generateTanhRandom(20000, 60000));
        break;
    default:
        break;
    }
}

void KatagoInteractor::setTimeModeFromInt(int newTimeMode)
{
    setTimeMode(KatagoInteractor::TimeMode(newTimeMode));
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
    disconnect(katagoProcess, &QProcess::readyRead, this, &KatagoInteractor::analyzeKatagoOutput);
    connect(katagoProcess, &QProcess::readyRead, this, &KatagoInteractor::analyzeKatagoInit);
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
            qWarning() << Q_FUNC_INFO << QStringLiteral("stone's color is none");
            break;
        }
        stoneJsonArray.append(pointToGTP(i.getPoint()));
        stonesMoveJsonArray.append(stoneJsonArray);
    }
    return stonesMoveJsonArray;
}

void KatagoInteractor::emitBestMove()
{
    if (m_bestMove.getColor() == StoneData::None || QDateTime::currentMSecsSinceEpoch() - lastMoveTime < 1000)
    {
        QTimer::singleShot(100, this, &KatagoInteractor::emitBestMove);
        return;
    }
    emit bestMove(m_bestMove);
    m_bestMove = StoneData();
}
