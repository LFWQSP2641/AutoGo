#include "KatagoInteractor.h"

#include "Settings.h"

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
      katagoProcess(new QProcess(this)),
      eventLoop(new QEventLoop(this)),
      timeOut(new QTimer(this))
{
    timeOut->setSingleShot(true);
    timeOut->setInterval(1000);
    connect(katagoProcess, &QProcess::readyRead, eventLoop, &QEventLoop::quit);
    connect(timeOut, &QTimer::timeout, eventLoop, &QEventLoop::quit);
}

void KatagoInteractor::init()
{
    disconnect(katagoProcess, &QProcess::readyRead, this, &KatagoInteractor::analyzeKatagoOutput);
    katagoProcess->setProcessChannelMode(QProcess::MergedChannels);
    katagoProcess->start(
        Settings::getSingletonSettings()->kataGoPath(),
        QProcess::splitCommand(Settings::getSingletonSettings()->kataGoCommand()));
    while (true)
    {
        timeOut->start();
        eventLoop->exec();
        bytes.append(katagoProcess->readAllStandardOutput());
        if (bytes.contains(QByteArrayLiteral("Started, ready to begin handling requests")))
        {
            bytes.clear();
            connect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoInteractor::analyzeKatagoOutput);
            emit initFinished(true);
            break;
        }
        else if (katagoProcess->state() == QProcess::NotRunning)
        {
            bytes.clear();
            emit initFinished(false);
            break;
        }
    }
}

void KatagoInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    m_boardData = BoardData();
}

void KatagoInteractor::stopAnalyze()
{
    qDebug() << Q_FUNC_INFO;
    katagoProcess->write(QByteArrayLiteral("{\"id\":\"stopAnalyze\",\"action\":\"terminate_all\"}\n"));
}

void KatagoInteractor::move(const BoardData &boardData)
{
    if (boardData.hasUnexpected())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("boardData hasUnexpected");
        return;
    }
    if (boardData.getMyStoneColor() == boardData.getLastMoveStone().getColor())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("is my turn, not analysis");
        return;
    }
    qDebug() << Q_FUNC_INFO << boardData.getLastMoveStone();
    m_boardData = boardData;
    QJsonObject jsonObject;
    jsonObject.insert(QStringLiteral("id"), m_boardData.getUuid());
    jsonObject.insert(QStringLiteral("initialStones"), stoneDataListToJsonArray(m_boardData.getInitialStonesArray()));
    jsonObject.insert(QStringLiteral("moves"), stoneDataListToJsonArray(m_boardData.getMoveStonesArray()));
    jsonObject.insert(QStringLiteral("rules"), QStringLiteral("chinese-ogs"));
    jsonObject.insert(QStringLiteral("komi"), 7.5);
    jsonObject.insert(QStringLiteral("boardXSize"), 19);
    jsonObject.insert(QStringLiteral("boardYSize"), 19);
    // 开局加快下棋速度
    if (m_boardData.getInitialStonesArray().size() + m_boardData.getMoveStonesArray().size() < 11)
        jsonObject.insert(QStringLiteral("maxVisits"), 128);
    const QByteArray data(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact).append(10));
    qDebug() << data;
    katagoProcess->write(data);
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

void KatagoInteractor::analyzeKatagoOutput()
{
    // 追加Katago进程输出
    bytes.append(katagoProcess->readAllStandardOutput());
#if 1
    if (!bytes.endsWith(10))
    {
        qDebug() << Q_FUNC_INFO << bytes;
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
            qDebug() << Q_FUNC_INFO << error.errorString() << bytes;
            bytes.clear();
            return;
        }
    }
    bytes.clear();
    if (jsonObject.value(QStringLiteral("id")).toString() != m_boardData.getUuid())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("id is not equal");
        return;
    }
    const auto bestMoveInfo(jsonObject.value(QStringLiteral("moveInfos")).toArray().at(0).toObject());
    if (bestMoveInfo.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("bestMoveInfo is empty");
        return;
    }
    const auto bestMovePoint(gptToPoint(bestMoveInfo.value(QStringLiteral("move")).toString()));
    qDebug() << Q_FUNC_INFO << bestMovePoint;
    emit bestMove(StoneData(m_boardData.getLastMoveStone().getColor() ==
                                    StoneData::StoneColor::Black
                                ? StoneData::StoneColor::White
                                : StoneData::StoneColor::Black,
                            bestMovePoint));
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

        emit bestMove(StoneData(m_boardData.getLastMoveStone().getColor() ==
                                        StoneData::StoneColor::Black
                                    ? StoneData::StoneColor::White
                                    : StoneData::StoneColor::Black,
                                gptToPoint(gtp)));
        // 清空bytes
        bytes.clear();
    }
    else
    {
        // 若未找到符合条件的结果，则截取bytes从index1处开始的子串
        bytes = bytes.mid(index1);
    }
#endif
}
