#include "BoardAnalyzer.h"

#include "InteractiveInterface/AdbScreencaptor.h"
#include "InteractiveInterface/MumuScreencaptor.h"
#include "Settings.h"
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
    { QStringLiteral("GameOverDialogCloseButton"), QPoint(975, 330) },
    { QStringLiteral("LevelUpDialogCloseButton"),  QPoint(935, 205) },
    { QStringLiteral("Backutton"),                 QPoint(30,  110) },
    { QStringLiteral("Backutton2"),                QPoint(30,  110) },
    { QStringLiteral("ConfirmDefeatDialog"),       QPoint(200, 850) },
    { QStringLiteral("DurationChoiceDialog"),      QPoint(440, 250) },
    { QStringLiteral("MainPage"),                  QPoint(0,   1520)},
    { QStringLiteral("MatchDialog"),               QPoint(450, 510) },
    { QStringLiteral("PlayingPage"),               QPoint(0,   1770)},
    { QStringLiteral("PlayingPageWithMove"),       QPoint(400, 1640)},
    { QStringLiteral("PlayingPageWithMove2"),      QPoint(400, 1640)},
    { QStringLiteral("RequestCountingDialog"),     QPoint(200, 850) },
    { QStringLiteral("RequestDrawDialog"),         QPoint(200, 850) },
    { QStringLiteral("RequestRematchDialog"),      QPoint(200, 850) },
    { QStringLiteral("RequestResumeBattleDialog"), QPoint(200, 850) },
    { QStringLiteral("RequestUndoDialog"),         QPoint(200, 850) },
    { QStringLiteral("TipDialogWithButton"),       QPoint(490, 710) }
};

BoardAnalyzer::BoardAnalyzer(QObject *parent)
    : QObject{ parent },
      toStop(false)
{
    if (Settings::getSingletonSettings()->mumuEnable())
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("mumuEnable");
        screencaptor = new MumuScreencaptor;
    }
    else
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("adbEnable");
        screencaptor = new AdbScreencaptor;
    }
}

BoardAnalyzer::AppNavigation BoardAnalyzer::appNavigationAnalyze(const cv::Mat &image)
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
    if (funcEqual(QStringLiteral("GameOverDialogCloseButton")))
        return BoardAnalyzer::gameOverDialog;
    if (funcEqual(QStringLiteral("LevelUpDialogCloseButton")))
        return BoardAnalyzer::levelUpDialog;
    if (funcEqual(QStringLiteral("PlayingPage")))
    {
        if (image.at<cv::Vec3b>(946, 288) == cv::Vec3b(78, 111, 80))
            return BoardAnalyzer::tipDialog;
        return BoardAnalyzer::playingPage;
    }
    if (funcEqual(QStringLiteral("PlayingPageWithMove")) || funcEqual(QStringLiteral("PlayingPageWithMove2")))
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
        if (m_boardData.getMyStoneColor() != boardData.getMyStoneColor() &&
            m_boardData.getMyStoneColor() != StoneData::StoneColor::None &&
            m_boardData.getInitialStonesArray().size() + m_boardData.getMoveStonesArray().size() == 0)
        {
            emit myStoneColorUpdate(m_boardData.getMyStoneColor());
            return;
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
        if (result == AppNavigation::playingPage)
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

bool BoardAnalyzer::checkGameState(AppNavigation navigation)
{
    qDebug() << Q_FUNC_INFO << navigation;
    switch (navigation)
    {
    case AppNavigation::gameOverDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toCloseGameOverDialog");
        emit toCloseGameOverDialog();
        break;
    case AppNavigation::levelUpDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toCloseLevelUpDialog");
        emit toCloseLevelUpDialog();
        break;
    case AppNavigation::analysisPage:
    case AppNavigation::pageWithBack:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toBackToMain");
        emit toBackToMain();
        break;
    case AppNavigation::requestRematchDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toAcceptRequest");
        emit toAcceptRequest();
        break;
    case AppNavigation::requestResumeBattleDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toRejectRequest");
        emit toRejectRequest();
        break;
    case AppNavigation::cancelResumeBattleDialog:
        qDebug() << Q_FUNC_INFO << QStringLiteral("toCloseRequest");
        emit toCloseRequest();
        break;
    case AppNavigation::mainPage:
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
        if (appNavigationAnalyze(image.value()) == AppNavigation::playingPage)
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
    // WARNING:
    // 未处理悔棋情况
    // 当进入倒计时才识别时, 可能会出现lastMoveStone为空(影响GTP)
    // initialStonesArray与lastMoveStone和moveStonesArray冲突(影响Analysis)

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
            }
            else if (pixelValue[0] > 200) // 白子
            {
                currentPiece = 2;
            }
            // 更新二维数组中的棋盘状态
            m_boardData.boardDataArray[i][j] = currentPiece;
            if (currentPiece == 0)
                continue;

            const QList<cv::Vec3b> pixelCValues{ image.at<cv::Vec3b>(checkCY, checkCX),
                                                 image.at<cv::Vec3b>(checkCY + 5, checkCX),
                                                 image.at<cv::Vec3b>(checkCY, checkCX + 5) };
            const unsigned char lastMovePixelValue(currentPiece == 1 ? 255 : 0);
            const auto isLastMove(pixelCValues.at(0)[0] == lastMovePixelValue && pixelCValues.at(1)[0] == lastMovePixelValue && pixelCValues.at(2)[0] == lastMovePixelValue);
            const StoneData::StoneColor lastMoveColor(currentPiece == 1 ? StoneData::StoneColor::Black : StoneData::StoneColor::White);
            if (isLastMove)
            {
                m_boardData.lastMoveStone.setColor(lastMoveColor);
                m_boardData.lastMoveStone.setPoint(QPoint(j, i));
                if (m_boardData.moveStonesArray.isEmpty() || m_boardData.moveStonesArray.last() != m_boardData.lastMoveStone)
                    m_boardData.moveStonesArray.append(m_boardData.lastMoveStone);
            }
            else if (needInitialStones)
            {
                m_boardData.initialStonesArray.append(StoneData(lastMoveColor, QPoint(j, i)));
            }
        }
    }
    // 己方执黑第一步时, needInitialStones会为true导致initialStonesArray异常
    if (needInitialStones && m_boardData.initialStonesArray.size() == 1)
    {
        m_boardData.moveStonesArray = m_boardData.initialStonesArray;
        m_boardData.initialStonesArray.clear();
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
    if (result != AppNavigation::playingPage)
    {
        switch (result)
        {
        case AppNavigation::playingPageWithMove:
            m_boardData.isMoving = true;
            break;
        case AppNavigation::requestDrawDialog:
            m_boardData.requestDraw = true;
            break;
        case AppNavigation::requestCountingDialog:
            m_boardData.requestCounting = true;
            break;
        case AppNavigation::requestUndoDialog:
            m_boardData.requestUndo = true;
            break;
        case AppNavigation::gameOverDialog:
            qDebug() << Q_FUNC_INFO << QStringLiteral("gameOver");
            m_boardData.gameOver = true;
            break;
        case AppNavigation::otherDialog:
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
