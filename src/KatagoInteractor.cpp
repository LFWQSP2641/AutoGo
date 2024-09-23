#include "KatagoInteractor.h"

#include "Settings.h"

#include <QDebug>
#include <QEventLoop>
#include <QProcess>
#include <QTimer>

KatagoInteractor::KatagoInteractor(QObject *parent)
    : QObject{ parent },
      myStoneColor(StoneData::StoneColor::None),
      katagoProcess(new QProcess(this)),
      eventLoop(new QEventLoop(this)),
      timeOut(new QTimer(this))
{
    timeOut->setSingleShot(true);
    timeOut->setInterval(1000);
    connect(katagoProcess, &QProcess::readyRead, eventLoop, &QEventLoop::quit);
    connect(timeOut, &QTimer::timeout, eventLoop, &QEventLoop::quit);
}

StoneData KatagoInteractor::getLastMoveStone() const
{
    return lastMoveStone;
}

void KatagoInteractor::init()
{
    disconnect(katagoProcess, &QProcess::readyRead, this, &KatagoInteractor::analyzeKatagoOutput);
    // katagoProcess->setWorkingDirectory(QStringLiteral("D:/Software/GoAI/katago"));
    katagoProcess->setProcessChannelMode(QProcess::MergedChannels);
    katagoProcess->start(
        Settings::getSingletonSettings()->kataGoPath(),
        QProcess::splitCommand(Settings::getSingletonSettings()->kataGoCommand()));
    while (true)
    {
        timeOut->start();
        eventLoop->exec();
        bytes.append(katagoProcess->readAllStandardOutput());
        if (bytes.contains(QByteArrayLiteral("GTP ready, beginning main protocol loop")))
        {
            bytes.clear();
            katagoProcess->write(QByteArrayLiteral("komi 7.5\n"));
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
    myStoneColor = StoneData::StoneColor::None;
    lastMoveStone = StoneData();
    setBestMove(StoneData());
    katagoProcess->write(QByteArrayLiteral("clear_board\n"));
    katagoProcess->write(QByteArrayLiteral("komi 7.5\n"));
}

void KatagoInteractor::stopAnalyze()
{
    qDebug() << Q_FUNC_INFO;
    katagoProcess->write(QByteArrayLiteral("stop\n"));
}

void KatagoInteractor::move(const StoneData &stoneData)
{
    qDebug() << Q_FUNC_INFO << stoneData;
    lastMoveStone = stoneData;
    QByteArray data;
    data.append(QByteArrayLiteral("play "));
    data.append(stoneData.getColor() == StoneData::StoneColor::Black
                    ? QByteArrayLiteral("B ")
                    : QByteArrayLiteral("W "));
    data.append(pointToGTP(stoneData.getPoint()).toUtf8());
    data.append(QByteArrayLiteral("\n"));
    katagoProcess->write(data);
    if (stoneData.getColor() != myStoneColor)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("kata-analyze 10");
        katagoProcess->write(QByteArrayLiteral("kata-analyze 10\n"));
    }
}

StoneData KatagoInteractor::getBestMove()
{
    qDebug() << Q_FUNC_INFO << m_bestMove;
    emit bestMove(m_bestMove);
    return m_bestMove;
}

StoneData::StoneColor KatagoInteractor::getMyStoneColor() const
{
    qDebug() << Q_FUNC_INFO;
    return myStoneColor;
}

void KatagoInteractor::setMyStoneColor(StoneData::StoneColor newMyStoneColor)
{
    qDebug() << Q_FUNC_INFO << newMyStoneColor;
    if (myStoneColor == newMyStoneColor)
        return;
    myStoneColor = newMyStoneColor;
    emit myStoneColorChanged();
}

void KatagoInteractor::setBestMove(const StoneData &newBestMove)
{
    if (m_bestMove == newBestMove)
        return;
    m_bestMove = newBestMove;
    emit bestMoveChanged(m_bestMove);
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

void KatagoInteractor::analyzeKatagoOutput()
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
        StoneData bestStoneData;
        bestStoneData.setPoint(gptToPoint(gtp));
        bestStoneData.setColor(lastMoveStone.getColor() == StoneData::StoneColor::Black
                                   ? StoneData::StoneColor::White
                                   : StoneData::StoneColor::Black);
        setBestMove(bestStoneData);

        // 清空bytes
        bytes.clear();
    }
    else
    {
        // 若未找到符合条件的结果，则截取bytes从index1处开始的子串
        bytes = bytes.mid(index1);
    }
}
