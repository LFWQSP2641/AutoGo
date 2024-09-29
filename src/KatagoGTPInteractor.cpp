#include "KatagoGTPInteractor.h"

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

KatagoGTPInteractor::KatagoGTPInteractor(QObject *parent)
    : KatagoInteractor{ parent },
      timer(new QTimer(this))
{
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &KatagoGTPInteractor::emitBestMove);
}

void KatagoGTPInteractor::init()
{
    disconnect(katagoProcess, &QProcess::readyRead, this, &KatagoGTPInteractor::analyzeKatagoOutput);
    // katagoProcess->setWorkingDirectory(QStringLiteral("D:/Software/GoAI/katago"));
    katagoProcess->setProcessChannelMode(QProcess::MergedChannels);
    katagoProcess->start(
        Settings::getSingletonSettings()->kataGoPath(),
        QProcess::splitCommand(Settings::getSingletonSettings()->kataGoGTPCommand()));
    while (true)
    {
        timeOut->start();
        eventLoop->exec();
        bytes.append(katagoProcess->readAllStandardOutput());
        if (bytes.contains(QByteArrayLiteral("GTP ready, beginning main protocol loop")))
        {
            bytes.clear();
            katagoProcess->write(QByteArrayLiteral("komi 7.5\n"));
            connect(katagoProcess, &QProcess::readyReadStandardOutput, this, &KatagoGTPInteractor::analyzeKatagoOutput);
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

void KatagoGTPInteractor::clearBoard()
{
    qDebug() << Q_FUNC_INFO;
    KatagoInteractor::clearBoard();
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
    if (boardData.hasUnexpected())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("boardData hasUnexpected");
        return;
    }
    if ((!boardData.getInitialStonesArray().isEmpty()) && m_boardData.getInitialStonesArray() != boardData.getInitialStonesArray())
    {
        QByteArray data;
        for (const auto &i : boardData.getInitialStonesArray())
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
    data.append(boardData.getLastMoveStone().getColor() == StoneData::StoneColor::Black
                    ? QByteArrayLiteral("B ")
                    : QByteArrayLiteral("W "));
    data.append(pointToGTP(boardData.getLastMoveStone().getPoint()).toUtf8());
    data.append(QByteArrayLiteral("\n"));
    qDebug() << Q_FUNC_INFO << data;
    katagoProcess->write(data);
    m_boardData = boardData;
    if (boardData.getMyStoneColor() != boardData.getLastMoveStone().getColor())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("kata-analyze 10");
        katagoProcess->write(QByteArrayLiteral("kata-analyze 10\n"));
        startTimer();
    }
}

void KatagoGTPInteractor::startTimer()
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
        const QByteArray searchString3(QByteArrayLiteral(" "));
        auto index3 = bytes.indexOf(searchString3, index1 + searchString1.size());

        // 提取GTP字符串并转化为最佳下法点
        auto gtp = bytes.sliced(index1 + searchString1.size(), index3 - index1 - searchString1.size());

        // 设置最佳下法的点和颜色
        m_bestMove.setPoint(gptToPoint(gtp));
        m_bestMove.setColor(m_boardData.getLastMoveStone().getColor() == StoneData::StoneColor::Black
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

void KatagoGTPInteractor::emitBestMove()
{
    if (m_bestMove.getColor() == StoneData::None)
    {
        QTimer::singleShot(100, this, &KatagoGTPInteractor::emitBestMove);
        return;
    }
    emit bestMove(m_bestMove);
    m_bestMove = StoneData();
}
