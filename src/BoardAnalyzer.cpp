#include "BoardAnalyzer.h"

#include "Util.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <QUuid>
#include <opencv2/opencv.hpp>

QHash<QString, QPoint> BoardAnalyzer::templateImagePoints = {
    {QStringLiteral("AnalysisPage"),               QPoint(0,   1770)},
    { QStringLiteral("AnalysisPage2"),             QPoint(0,   1770)},
    { QStringLiteral("CancelResumeBattleDialog"),  QPoint(200, 850) },
    { QStringLiteral("CloseButton"),               QPoint(975, 330) },
    { QStringLiteral("Backutton"),                 QPoint(30,  110) },
    { QStringLiteral("Backutton2"),                QPoint(30,  110) },
    { QStringLiteral("ConfirmDefeatDialog"),       QPoint(200, 850) },
    { QStringLiteral("DurationChoiceDialog"),      QPoint(440, 250) },
    { QStringLiteral("MainPage"),                  QPoint(0,   1520)},
    { QStringLiteral("MatchDialog"),               QPoint(450, 510) },
    { QStringLiteral("PlayingPage"),               QPoint(0,   1770)},
    { QStringLiteral("PlayingPageWithMove"),       QPoint(400, 1640)},
    { QStringLiteral("RequestCountingDialog"),     QPoint(200, 850) },
    { QStringLiteral("RequestDrawDialog"),         QPoint(200, 850) },
    { QStringLiteral("RequestRematchDialog"),      QPoint(200, 850) },
    { QStringLiteral("RequestResumeBattleDialog"), QPoint(200, 850) },
    { QStringLiteral("RequestUndoDialog"),         QPoint(200, 850) },
    { QStringLiteral("TipDialogWithButton"),       QPoint(490, 710) }
};

BoardAnalyzer::BoardAnalyzer(QObject *parent)
    : QObject{ parent },
      screencaptor(new MumuScreencaptor(this)),
      toStop(false)
{
}

BoardAnalyzer::appNavigation BoardAnalyzer::appNavigationAnalyze(const cv::Mat &image)
{
    auto funcEqual([image](const QString &name) -> bool
                   { return Util::isRegionEqual(image, name, templateImagePoints.value(name)); });

    if (funcEqual(QStringLiteral("TipDialogWithButton")))
    {
        if (funcEqual(QStringLiteral("RequestCountingDialog")))
            return BoardAnalyzer::requestCountingDialog;
        if (funcEqual(QStringLiteral("RequestDrawDialog")))
            return BoardAnalyzer::requestDrawDialog;
        if (funcEqual(QStringLiteral("RequestRematchDialog")))
            return BoardAnalyzer::requestRematchDialog;
        if (funcEqual(QStringLiteral("RequestResumeBattleDialog")))
            return BoardAnalyzer::requestResumeBattleDialog;
        if (funcEqual(QStringLiteral("RequestUndoDialog")))
            return BoardAnalyzer::requestUndoDialog;
        if (funcEqual(QStringLiteral("ConfirmDefeatDialog")))
            return BoardAnalyzer::confirmDefeatDialog;
        if (funcEqual(QStringLiteral("CancelResumeBattleDialog")))
            return BoardAnalyzer::cancelResumeBattleDialog;
        return BoardAnalyzer::otherDialog;
    }
    if (funcEqual(QStringLiteral("DurationChoiceDialog")))
        return BoardAnalyzer::durationChoiceDialog;
    if (funcEqual(QStringLiteral("MatchDialog")))
        return BoardAnalyzer::matchDialog;
    if (funcEqual(QStringLiteral("CloseButton")))
        return BoardAnalyzer::onlyCloseButtonDialog;
    if (funcEqual(QStringLiteral("PlayingPage")))
    {
        if (image.at<cv::Vec3b>(946, 288) == cv::Vec3b(78, 111, 80))
            return BoardAnalyzer::tipDialog;
        return BoardAnalyzer::playingPage;
    }
    if (funcEqual(QStringLiteral("PlayingPageWithMove")))
        return BoardAnalyzer::playingPageWithMove;
    if (funcEqual(QStringLiteral("AnalysisPage")) || funcEqual(QStringLiteral("AnalysisPage2")))
        return BoardAnalyzer::analysisPage;
    if (funcEqual(QStringLiteral("MainPage")))
        return BoardAnalyzer::mainPage;
    if (funcEqual(QStringLiteral("Backutton")) || funcEqual(QStringLiteral("Backutton2")))
        return BoardAnalyzer::pageWithBack;

    const auto filePath(QCoreApplication::applicationDirPath()
                            .append(QStringLiteral("/debug/"))
                            .append(QUuid::createUuid()
                                        .toString(QUuid::WithoutBraces)
                                        .append(QStringLiteral(".png"))));
    cv::imwrite(filePath.toStdString(), image);
    qWarning() << Q_FUNC_INFO << "not matching" << filePath;
    return BoardAnalyzer::unknownPage;
}

BoardData BoardAnalyzer::analyzeBoard()
{
    auto image(screencaptor->screencap());
    if (!image)
    {
        qWarning() << Q_FUNC_INFO << QStringLiteral("image is null");
        return m_boardData;
    }

    if (checkGameStatus(image.value()))
    {
        m_boardData.myStoneColor = this->getMyStoneColor(image.value());
        isTurnToPlay(image.value());
        getBoardArray(image.value());
    }
    emit analyzeBoardData(m_boardData);
    return m_boardData;
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
    screencaptor->init();
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
        const auto boardData(m_boardData);
        analyzeBoard();
        if (boardData == m_boardData)
            continue;
        if (m_boardData.getIsMoving())
        {
            qDebug() << Q_FUNC_INFO << QStringLiteral("Stone is moving");
            continue;
        }
        const bool moved(m_boardData.getLastMoveStone() != boardData.getLastMoveStone() &&
                         m_boardData.getLastMoveStone().getPoint() != QPoint(-1, -1));
        if (m_boardData.hasUnexpected() || moved)
            emit analyzeIndefinitelyData(m_boardData);
        if (m_boardData.hasUnexpected())
            break;
        if (moved)
        {
            qDebug() << Q_FUNC_INFO << QStringLiteral("LastMoveStone:") << m_boardData.getLastMoveStone();
            emit lastMoveStone(m_boardData.getLastMoveStone());
        }
    }
    qDebug() << Q_FUNC_INFO << QStringLiteral("exit");
    emit analyzeStoped();
}

void BoardAnalyzer::stop()
{
    qDebug() << Q_FUNC_INFO;
    toStop = true;
}

void BoardAnalyzer::startGame()
{
    while (true)
    {
        const auto image(screencaptor->screencap());
        if (!image)
        {
            qWarning() << Q_FUNC_INFO << QStringLiteral("image is null");
            continue;
        }
        const auto result(appNavigationAnalyze(image.value()));
        if (result == appNavigation::playingPage)
        {
            qDebug() << Q_FUNC_INFO << "game started";
            emit gameStarted();
            break;
        }
        if (checkGameState(result))
        {
            waitForGameMatching();
        }
        else
        {
            // 等待界面响应, 避免多次点击
            QThread::sleep(1);
        }
    }
}

bool BoardAnalyzer::checkGameState(appNavigation navigation)
{
    qDebug() << Q_FUNC_INFO << navigation;
    switch (navigation)
    {
    case appNavigation::onlyCloseButtonDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toCloseGameOverDialog");
        emit toCloseGameOverDialog();
        break;
    case appNavigation::analysisPage:
    case appNavigation::pageWithBack:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toBackToMain");
        emit toBackToMain();
        break;
    case appNavigation::requestRematchDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toAcceptRequest");
        emit toAcceptRequest();
        break;
    case appNavigation::requestResumeBattleDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toRejectRequest");
        emit toRejectRequest();
        break;
    case appNavigation::mainPage:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toMatchGame");
        emit toMatchGame();
        return true;
        break;
    default:
        break;
    }
    return false;
}

void BoardAnalyzer::waitForGameMatching()
{
    qDebug() << Q_FUNC_INFO << QStringLiteral("enter");
    const auto startTime(QDateTime::currentSecsSinceEpoch());
    do
    {
        auto image(screencaptor->screencap());
        if (!image)
        {
            qWarning() << Q_FUNC_INFO << QStringLiteral("image is null");
            continue;
        }
        if (appNavigationAnalyze(image.value()) == appNavigation::playingPage)
            break;
    } while (QDateTime::currentSecsSinceEpoch() - startTime < 60);
    qDebug() << Q_FUNC_INFO << QStringLiteral("exit");
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

void BoardAnalyzer::isTurnToPlay(const cv::Mat &image)
{
    const cv::Vec3b pixelValue = image.at<cv::Vec3b>(305, 305);
    m_boardData.needMove = (pixelValue == cv::Vec3b(118, 254, 255));
}

void BoardAnalyzer::getBoardArray(const cv::Mat &image)
{
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
    const int offsetX = 15; // x轴偏移量
    const int offsetY = 15; // y轴偏移量
    const int offsetCX = 8; // 监测点x轴偏移量
    const int offsetCY = 8; // 监测点y轴偏移量

    const auto needInitialStones(m_boardData.getInitialStonesArray().isEmpty() && m_boardData.getMoveStonesArray().isEmpty());

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
                const QList<cv::Vec3b> pixelCValues{ image.at<cv::Vec3b>(checkCY, checkCX),
                                                     image.at<cv::Vec3b>(checkCY + 5, checkCX),
                                                     image.at<cv::Vec3b>(checkCY, checkCX + 5) };
                if (pixelCValues.at(0)[0] == 255 && pixelCValues.at(1)[0] == 255 && pixelCValues.at(2)[0] == 255)
                {
                    m_boardData.lastMoveStone.setColor(StoneData::StoneColor::Black);
                    m_boardData.lastMoveStone.setPoint(QPoint(j, i));
                    if (m_boardData.moveStonesArray.isEmpty() || m_boardData.moveStonesArray.last() != m_boardData.lastMoveStone)
                        m_boardData.moveStonesArray.append(m_boardData.lastMoveStone);
                }
                else if (needInitialStones)
                {
                    m_boardData.initialStonesArray.append(StoneData(StoneData::StoneColor::Black, QPoint(j, i)));
                }
            }
            else if (pixelValue[0] > 200) // 白子
            {
                currentPiece = 2;
                const QList<cv::Vec3b> pixelCValues{ image.at<cv::Vec3b>(checkCY, checkCX),
                                                     image.at<cv::Vec3b>(checkCY + 5, checkCX),
                                                     image.at<cv::Vec3b>(checkCY, checkCX + 5) };
                if (pixelCValues.at(0)[0] == 0 && pixelCValues.at(1)[0] == 0 && pixelCValues.at(2)[0] == 0)
                {
                    m_boardData.lastMoveStone.setColor(StoneData::StoneColor::White);
                    m_boardData.lastMoveStone.setPoint(QPoint(j, i));
                    if (m_boardData.moveStonesArray.isEmpty() || m_boardData.moveStonesArray.last() != m_boardData.lastMoveStone)
                        m_boardData.moveStonesArray.append(m_boardData.lastMoveStone);
                }
                else if (needInitialStones)
                {
                    m_boardData.initialStonesArray.append(StoneData(StoneData::StoneColor::White, QPoint(j, i)));
                }
            }

            // 更新二维数组中的棋盘状态
            m_boardData.boardDataArray[i][j] = currentPiece;
        }
    }
}

bool BoardAnalyzer::checkGameStatus(const cv::Mat &image)
{
    m_boardData.isMoving = false;
    m_boardData.requestDraw = false;
    m_boardData.requestCounting = false;
    m_boardData.requestUndo = false;
    m_boardData.gameOver = false;
    m_boardData.unknownUnexpected = false;
    const auto result(appNavigationAnalyze(image));
    if (result != appNavigation::playingPage)
    {
        switch (result)
        {
        case appNavigation::playingPageWithMove:
            m_boardData.isMoving = true;
            break;
        case appNavigation::requestDrawDialog:
            m_boardData.requestDraw = true;
            break;
        case appNavigation::requestCountingDialog:
            m_boardData.requestCounting = true;
            break;
        case appNavigation::requestUndoDialog:
            m_boardData.requestUndo = true;
            break;
        case appNavigation::onlyCloseButtonDialog:
            qDebug() << Q_FUNC_INFO << QStringLiteral("gameOver");
            m_boardData.gameOver = true;
            break;
        case appNavigation::otherDialog:
            m_boardData.unknownUnexpected = true;
            break;
        default:
            qDebug() << Q_FUNC_INFO << result;
            break;
        }
        return false;
    }
    return true;
}
