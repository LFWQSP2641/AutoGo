#include "KatagoAnalysisInteractor.h"

#include "Settings.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>
#include <QTimer>

KatagoAnalysisInteractor::KatagoAnalysisInteractor(QObject *parent)
    : KatagoInteractor{ parent }
{
}

void KatagoAnalysisInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    m_boardData = BoardData();
}

void KatagoAnalysisInteractor::stopAnalyze()
{
    qDebug() << Q_FUNC_INFO;
    katagoProcess->write(QByteArrayLiteral("{\"id\":\"stopAnalyze\",\"action\":\"terminate_all\"}\n"));
}

void KatagoAnalysisInteractor::move(const BoardData &boardData)
{
    if (boardData.hasUnexpected())
    {
        qDebug() << QStringLiteral("boardData hasUnexpected");
        return;
    }
    if (boardData.getMyStoneColor() == boardData.getLastMoveStone().getColor())
    {
        qDebug() << QStringLiteral("is my turn, not analysis");
        return;
    }
    lastMoveTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << boardData.getLastMoveStone();
    m_boardData = boardData;
    QJsonObject jsonObject;
    jsonObject.insert(QStringLiteral("id"), m_boardData.getUuid());
    jsonObject.insert(QStringLiteral("initialStones"), stoneDataListToJsonArray(m_boardData.getInitialStonesArray()));
    jsonObject.insert(QStringLiteral("moves"), stoneDataListToJsonArray(m_boardData.getMoveStonesArray()));
    jsonObject.insert(QStringLiteral("rules"), QStringLiteral("chinese-ogs"));
    jsonObject.insert(QStringLiteral("komi"), 7.5);
    jsonObject.insert(QStringLiteral("boardXSize"), 19);
    jsonObject.insert(QStringLiteral("boardYSize"), 19);

    if (Settings::getSingletonSettings()->kataGoSearchLimit())
    {
        double timeScaleFactor;
        switch (m_timeMode)
        {
        case KatagoInteractor::TimeMode::Medium:
            timeScaleFactor = 2;
            break;
        case KatagoInteractor::TimeMode::Long:
            timeScaleFactor = 3;
            break;
        case KatagoInteractor::TimeMode::Short:
        default:
            timeScaleFactor = 1;
            break;
        }

        const auto moveSize(m_boardData.getInitialStonesArray().size() + m_boardData.getMoveStonesArray().size());
        int maxVisits(Settings::getSingletonSettings()->kataGoMaxVisits() * timeScaleFactor);
        // 开局加快下棋速度
        if (moveSize < 10)
            maxVisits = maxVisits * (10 - moveSize) / 10;

        jsonObject.insert(QStringLiteral("maxVisits"), maxVisits);
    }
    else
    {
        jsonObject.insert(QStringLiteral("reportDuringSearchEvery"), static_cast<double>(reportIntervalMS) / double(1000));
        startTimer();
    }
    const QByteArray data(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact).append(10));
    qDebug() << data;
    katagoProcess->write(data);
}

QStringList KatagoAnalysisInteractor::getKataGoArgs() const
{
    return QProcess::splitCommand(Settings::getSingletonSettings()->kataGoAnalysisCommand());
}

void KatagoAnalysisInteractor::analyzeKatagoOutput()
{
    // 追加Katago进程输出
    bytes.append(katagoProcess->readAllStandardOutput());
#if 1
    if (!bytes.endsWith(10))
    {
        qDebug() << bytes;
        return;
    }
    QJsonParseError error;
    auto jsonObject(QJsonDocument::fromJson(bytes, &error).object());
    if (error.error != QJsonParseError::NoError)
    {
        const auto index(bytes.lastIndexOf(10, bytes.size() - 2));
        if (index != -1)
        {
            bytes = bytes.sliced(index + 1);
            jsonObject = QJsonDocument::fromJson(bytes, &error).object();
        }
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << error.errorString() << bytes;
            bytes.clear();
            return;
        }
    }
    bytes.clear();
    if (jsonObject.value(QStringLiteral("id")).toString() != m_boardData.getUuid())
    {
        qDebug() << QStringLiteral("id is not equal");
        return;
    }
    const auto bestMoveInfo(jsonObject.value(QStringLiteral("moveInfos")).toArray().at(0).toObject());
    if (bestMoveInfo.isEmpty())
    {
        qDebug() << QStringLiteral("bestMoveInfo is empty");
        return;
    }
    const auto bestMovePoint(gptToPoint(bestMoveInfo.value(QStringLiteral("move")).toString()));
    qDebug() << bestMovePoint;
    m_bestMove = StoneData(m_boardData.getLastMoveStone().getColor() ==
                                   StoneData::StoneColor::Black
                               ? StoneData::StoneColor::White
                               : StoneData::StoneColor::Black,
                           bestMovePoint);
#else
    // 定义要搜索的字符串
    const QByteArray searchString1(QByteArrayLiteral("moveInfos"));
    const QByteArray searchString2(QByteArrayLiteral("\"order\":0"));

    // 查找对应的索引
    auto index1 = bytes.lastIndexOf(searchString1);
    const auto index2 = bytes.lastIndexOf(searchString2);

    // 如果index1大于index2，继续向前查找
    if (index1 > index2)
        index1 = bytes.lastIndexOf(searchString1, index1 - 1);

    // 当index1小于index2时，提取信息并进行处理
    if (index1 < index2)
    {
        // 双引号
        const char searchString3(34);
        const auto index3 = bytes.indexOf(searchString3, index2 - 6);

        // 提取GTP字符串并转化为最佳下法点
        const auto gtp = bytes.sliced(index3 + 1, index2 - 3 - index3);

        // 设置最佳下法的点和颜色

        m_bestMove = StoneData(m_boardData.getLastMoveStone().getColor() ==
                                       StoneData::StoneColor::Black
                                   ? StoneData::StoneColor::White
                                   : StoneData::StoneColor::Black,
                               gptToPoint(gtp));
        // 清空bytes
        bytes.clear();
    }
    else
    {
        // 若未找到符合条件的结果，则截取bytes从index1处开始的子串
        bytes = bytes.mid(index1);
        return;
    }
#endif
    if (Settings::getSingletonSettings()->kataGoSearchLimit())
        emitBestMove();
}

void KatagoAnalysisInteractor::analyzeKatagoInit()
{
    bytes.append(katagoProcess->readAllStandardOutput());
    if (bytes.contains(QByteArrayLiteral("Started, ready to begin handling requests")))
    {
        bytes.clear();
        disconnect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoAnalysisInteractor::analyzeKatagoInit);
        connect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoAnalysisInteractor::analyzeKatagoOutput);
        emit initFinished(true);
    }
    else if (katagoProcess->state() == QProcess::NotRunning)
    {
        bytes.clear();
        emit initFinished(false);
    }
}
