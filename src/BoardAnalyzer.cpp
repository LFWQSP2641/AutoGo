#include "BoardAnalyzer.h"

#include "Settings.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QUuid>
#include <opencv2/opencv.hpp>

BoardAnalyzer::BoardAnalyzer(QObject *parent)
    : QObject{ parent },
      screencaptor(new MumuScreencaptor(this)),
      toStop(false)
{
}

BoardData BoardAnalyzer::analyzeBoard()
{
    auto image(screencaptor->screencap());
    if (!image)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("image is null");
        return BoardData();
    }

    BoardData boardData;

    if (checkGameStatus(image.value(), boardData))
    {
        boardData.myStoneColor = this->getMyStoneColor(image.value());
        isTurnToPlay(image.value(), boardData);
        getBoardArray(image.value(), boardData);
    }
    else
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("checkGameStatus return false");
    }
    emit analyzeBoardData(boardData);
    return boardData;
}

StoneData::StoneColor BoardAnalyzer::checkMyStoneColor()
{
    auto image(screencaptor->screencap());
    if (!image)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("image is null");
        emit myStoneColorUpdate(StoneData::StoneColor::None);
        return StoneData::StoneColor::None;
    }

    const auto myStoneColor(getMyStoneColor(image.value()));
    emit myStoneColorUpdate(myStoneColor);
    return myStoneColor;
}

void BoardAnalyzer::init()
{
    screencaptor->init(Settings::getSingletonSettings()->mumuPath(), Settings::getSingletonSettings()->mumuIndex());
}

void BoardAnalyzer::resetBoardData()
{
    m_boardData = BoardData();
}

void BoardAnalyzer::analyzeIndefinitely()
{
    qDebug() << Q_FUNC_INFO << QStringLiteral("enter");
    toStop = false;
    while (!toStop)
    {
        const auto boardData(analyzeBoard());
        if (m_boardData == boardData)
            continue;
        if (boardData.getIsMoving())
        {
            qDebug() << Q_FUNC_INFO << QStringLiteral("Stone is moving");
            continue;
        }
        emit analyzeIndefinitelyData(boardData);
        if (boardData.hasUnexpected())
            break;
        if (m_boardData.getLastMoveStone() != boardData.getLastMoveStone() &&
            boardData.getLastMoveStone().getPoint() != QPoint(-1, -1))
        {
            qDebug() << Q_FUNC_INFO << QStringLiteral("LastMoveStone:") << boardData.getLastMoveStone();
            emit lastMoveStone(boardData.getLastMoveStone());
        }
        m_boardData = std::move(boardData);
    }
    qDebug() << Q_FUNC_INFO << QStringLiteral("exit");
    emit analyzeStoped();
}

void BoardAnalyzer::stop()
{
    qDebug() << Q_FUNC_INFO;
    toStop = true;
}

void BoardAnalyzer::waitForGameStarting()
{
    const auto startTime(QDateTime::currentSecsSinceEpoch());
    do
    {
        auto image(screencaptor->screencap());
        if (!image)
        {
            qWarning() << QStringLiteral("image is null");
            continue;
        }
        if (image.value().at<cv::Vec3b>(324, 0) == cv::Vec3b(139, 202, 240) &&
            image.value().at<cv::Vec3b>(1814, 145) == cv::Vec3b(56, 88, 28))
            break;
    } while (QDateTime::currentSecsSinceEpoch() - startTime < 60);
    qDebug() << Q_FUNC_INFO << "game started";
    emit gameStarted();
}

StoneData::StoneColor BoardAnalyzer::getMyStoneColor(const cv::Mat &image)
{
    const cv::Vec3b pixelValue = image.at<cv::Vec3b>(200, 150);
    if (pixelValue == cv::Vec3b(31, 30, 26))
        return StoneData::StoneColor::Black;
    else if (pixelValue == cv::Vec3b(229, 225, 221))
        return StoneData::StoneColor::White;
    else
    {
        const auto filePath(QCoreApplication::applicationDirPath()
                                .append(QStringLiteral("/debug/"))
                                .append(QUuid::createUuid()
                                            .toString(QUuid::WithoutBraces)
                                            .append(QStringLiteral(".png"))));
        cv::imwrite(filePath.toStdString(), image);
        qWarning() << Q_FUNC_INFO
                   << QStringLiteral("myStoneColor check failed, rgb:")
                   << pixelValue[2] << pixelValue[1] << pixelValue[0]
                   << filePath;
        return StoneData::StoneColor::None;
    }
}

void BoardAnalyzer::isTurnToPlay(const cv::Mat &image, BoardData &boardData)
{
    const cv::Vec3b pixelValue = image.at<cv::Vec3b>(305, 305);
    boardData.needMove = (pixelValue == cv::Vec3b(118, 254, 255));
}

void BoardAnalyzer::getBoardArray(const cv::Mat &image, BoardData &boardData)
{
    if (image.at<cv::Vec3b>(1661, 513) == cv::Vec3b(33, 63, 1))
    {
        boardData.isMoving = true;
        return;
    }

    // 棋盘大小为19x19
    const int rows = 19;
    const int cols = 19;

    // 棋盘的四个已知角坐标
    const cv::Point2f topLeft(37, 361);        // 左上角
    const cv::Point2f bottomRight(1041, 1365); // 右下角

    // 计算每个格子的宽度和高度
    const float cellWidth = (bottomRight.x - topLeft.x) / (cols - 1);
    const float cellHeight = (bottomRight.y - topLeft.y) / (rows - 1);

    // 用于略微调整棋子检测点的位置，避免棋盘线干扰
    const int offsetX = 10; // x轴偏移量
    const int offsetY = 10; // y轴偏移量
    const int offsetCX = 8; // 监测点x轴偏移量
    const int offsetCY = 8; // 监测点y轴偏移量

    // 遍历棋盘的每个格子，判断当前格子内是否有棋子
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            // 计算当前格子的中心坐标
            const int centerX = static_cast<int>(topLeft.x + j * cellWidth);
            const int centerY = static_cast<int>(topLeft.y + i * cellHeight);

            // 调整检测点的坐标，避免棋盘线影响
            const int checkX = centerX - offsetX;
            const int checkY = centerY - offsetY;

            // 偏左下15像素的点，用于进一步检测（如果需要）
            const int checkCX = centerX + offsetCX;
            const int checkCY = centerY + offsetCY;

            const cv::Vec3b pixelValue = image.at<cv::Vec3b>(checkY, checkX);

            int currentPiece(0);

            // 根据检测到的棋子颜色绘制相应的圆点
            if (pixelValue[0] < 100) // 黑子
            {
                currentPiece = 1;
                const cv::Vec3b pixelCValue = image.at<cv::Vec3b>(checkCY, checkCX);
                if (pixelCValue[0] == 255)
                {
                    boardData.lastMoveStone.setColor(StoneData::StoneColor::Black);
                    boardData.lastMoveStone.setPoint(QPoint(j, i));
                }
            }
            else if (pixelValue[0] > 200) // 白子
            {
                currentPiece = 2;
                const cv::Vec3b pixelCValue = image.at<cv::Vec3b>(checkCY, checkCX);
                if (pixelCValue[0] == 0)
                {
                    boardData.lastMoveStone.setColor(StoneData::StoneColor::White);
                    boardData.lastMoveStone.setPoint(QPoint(j, i));
                }
            }

            // 更新二维数组中的棋盘状态
            boardData.boardDataArray[i][j] = currentPiece;
        }
    }
}

bool BoardAnalyzer::checkGameStatus(const cv::Mat &image, BoardData &boardData)
{
    bool hasUnexpected(image.at<cv::Vec3b>(713, 501) == cv::Vec3b(83, 104, 156));
    bool pass(image.at<cv::Vec3b>(946, 288) == cv::Vec3b(78, 111, 80));
    bool gameOver(image.at<cv::Vec3b>(333, 1000) == cv::Vec3b(133, 166, 125));
    if (hasUnexpected)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("hasUnexpected");
        const cv::Vec3b pixelValue = image.at<cv::Vec3b>(888, 475);
        if (pixelValue == cv::Vec3b(72, 107, 40))
            boardData.requestDraw = true;
        else if (pixelValue == cv::Vec3b(118, 146, 99))
            boardData.requestCounting = true;
        else if (pixelValue == cv::Vec3b(92, 123, 66))
            boardData.requestUndo = true;
        else
        {
            boardData.unknownUnexpected = true;
            const auto filePath(QCoreApplication::applicationDirPath()
                                    .append(QStringLiteral("/debug/"))
                                    .append(QUuid::createUuid()
                                                .toString(QUuid::WithoutBraces)
                                                .append(QStringLiteral(".png"))));
            cv::imwrite(filePath.toStdString(), image);
            qWarning() << Q_FUNC_INFO << "not matching" << filePath;
        }
    }
    else if (pass)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("pass");
        boardData.needMove = true;
    }
    else if (gameOver)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("gameOver");
        boardData.gameOver = true;
    }
    return !boardData.hasUnexpected();
}
