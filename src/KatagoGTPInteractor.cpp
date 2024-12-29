#include "KatagoGTPInteractor.h"

#include "Settings.h"

#include <QDebug>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>
#include <QTimer>

KatagoGTPInteractor::KatagoGTPInteractor(QObject *parent)
    : KatagoInteractor{ parent }
{
}

void KatagoGTPInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    this->KatagoInteractor::clearBoard();
    katagoProcess->write(QByteArrayLiteral("clear_board\n"));
    katagoProcess->write(QByteArrayLiteral("komi 7.5\n"));
}

void KatagoGTPInteractor::stopAnalyze()
{
    qDebug() << Q_FUNC_INFO;
    katagoProcess->write(QByteArrayLiteral("stop\n"));
}

void KatagoGTPInteractor::move(const BoardData &boardData)
{
    lastMoveTime = QDateTime::currentMSecsSinceEpoch();
    if ((!boardData.initialStonesArray().isEmpty()) && m_boardData.initialStonesArray() != boardData.initialStonesArray())
    {
        QByteArray data;
        for (const auto &i : boardData.initialStonesArray())
        {
            data.append(QByteArrayLiteral("play "));
            data.append(i.getColor() == StoneData::StoneColor::Black
                            ? QByteArrayLiteral("B ")
                            : QByteArrayLiteral("W "));
            data.append(pointToGTP(i.getPoint()).toUtf8());
            data.append(QByteArrayLiteral("\n"));
        }
        qDebug() << Q_FUNC_INFO << data;
        katagoProcess->write(data);
    }
    QByteArray data;
    data.append(QByteArrayLiteral("play "));
    data.append(boardData.lastMoveStone().getColor() == StoneData::StoneColor::Black
                    ? QByteArrayLiteral("B ")
                    : QByteArrayLiteral("W "));
    data.append(pointToGTP(boardData.lastMoveStone().getPoint()).toUtf8());
    data.append(QByteArrayLiteral("\n"));
    qDebug() << Q_FUNC_INFO << data;
    katagoProcess->write(data);
    m_boardData = boardData;
    if (boardData.myStoneColor() != boardData.lastMoveStone().getColor())
    {
        const QByteArray analyzeData(QByteArrayLiteral("kata-analyze ").append(QByteArray::number(reportIntervalMS / 10)).append(10));
        qDebug() << Q_FUNC_INFO << analyzeData;
        katagoProcess->write(analyzeData);
        startTimer();
    }
}

void KatagoGTPInteractor::clearCache()
{
    qDebug() << Q_FUNC_INFO;
    katagoProcess->write(QByteArrayLiteral("clear_cache\n"));
}

QStringList KatagoGTPInteractor::getKataGoArgs() const
{
    return QProcess::splitCommand(Settings::getSingletonSettings()->kataGoGTPCommand());
}

void KatagoGTPInteractor::analyzeKatagoOutput()
{
    // 追加Katago进程输出
    bytes.append(katagoProcess->readAllStandardOutput());

    // 定义要搜索的字符串
    const QByteArray searchString1(QByteArrayLiteral("\ninfo move "));
    const QByteArray searchString2(QByteArrayLiteral("order 0"));

    // 查找对应的索引
    auto index1 = bytes.lastIndexOf(searchString1);
    auto index2 = bytes.lastIndexOf(searchString2);

    // 如果index1大于index2，继续向前查找
    if (index1 > index2)
        index1 = bytes.lastIndexOf(searchString1, index1 - 1);

    // 当index1小于index2时，提取信息并进行处理
    if (index1 < index2)
    {
        const char searchString3(32);
        auto index3 = bytes.indexOf(searchString3, index1 + searchString1.size());

        // 提取GTP字符串并转化为最佳下法点
        auto gtp = bytes.sliced(index1 + searchString1.size(), index3 - index1 - searchString1.size());

        // 设置最佳下法的点和颜色
        m_bestMove.setPoint(gptToPoint(gtp));
        m_bestMove.setColor(m_boardData.lastMoveStone().getColor() == StoneData::StoneColor::Black
                                ? StoneData::StoneColor::White
                                : StoneData::StoneColor::Black);

        emit bestMoveUpdate(m_bestMove);

        // 清空bytes
        bytes.clear();
    }
    else
    {
        // 若未找到符合条件的结果，则截取bytes从index1处开始的子串
        bytes = bytes.mid(index1);
    }
}

void KatagoGTPInteractor::analyzeKatagoInit()
{
    bytes.append(katagoProcess->readAllStandardOutput());
    if (bytes.contains(QByteArrayLiteral("GTP ready, beginning main protocol loop")))
    {
        bytes.clear();
        disconnect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoGTPInteractor::analyzeKatagoInit);
        connect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoGTPInteractor::analyzeKatagoOutput);
        katagoProcess->write(QByteArrayLiteral("komi 7.5\n"));
        emit initFinished(true);
    }
    else if (katagoProcess->state() == QProcess::NotRunning)
    {
        bytes.clear();
        emit initFinished(false);
    }
}
