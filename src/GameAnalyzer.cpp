#include "GameAnalyzer.h"

#include "Global.h"
#include "Util.h"

#include <QDebug>
#include <QHash>
#include <QPoint>
#include <QThread>
#include <QTimer>

QHash<QString, QPoint> GameAnalyzer::templateImagePoints = {
    {QStringLiteral("AcceptCountingResult"),               QPoint(141, 1780)},
    { QStringLiteral("AIPKClose"),                         QPoint(980, 325) },
    { QStringLiteral("AnalysisPage"),                      QPoint(0,   1770)},
    { QStringLiteral("AnalysisPage2"),                     QPoint(0,   1770)},
    { QStringLiteral("CancelResumeBattleDialog"),          QPoint(200, 850) },
    { QStringLiteral("GameOverDialogCloseButton"),         QPoint(975, 330) },
    { QStringLiteral("GameOverDialogCloseButton2"),        QPoint(975, 330) },
    { QStringLiteral("LevelUpDialogCloseButton"),          QPoint(935, 205) },
    { QStringLiteral("Backutton"),                         QPoint(30,  110) },
    { QStringLiteral("Backutton2"),                        QPoint(30,  110) },
    { QStringLiteral("Backutton3"),                        QPoint(30,  110) },
    { QStringLiteral("ConfirmDefeatDialog"),               QPoint(200, 850) },
    { QStringLiteral("ConfirmDefeatDialog2"),              QPoint(200, 850) },
    { QStringLiteral("DurationChoiceDialog"),              QPoint(440, 250) },
    { QStringLiteral("MainPage"),                          QPoint(0,   1520)},
    { QStringLiteral("MatchDialog"),                       QPoint(450, 510) },
    { QStringLiteral("PlayingPage"),                       QPoint(0,   1770)},
    { QStringLiteral("PlayingPage2"),                      QPoint(0,   1770)},
    { QStringLiteral("PlayingPageWithMove"),               QPoint(400, 1640)},
    { QStringLiteral("PlayingPageWithMove2"),              QPoint(400, 1640)},
    { QStringLiteral("RequestCountingDialog"),             QPoint(200, 850) },
    { QStringLiteral("RequestCountingDialogMS"),           QPoint(200, 850) },
    { QStringLiteral("RequestIntelligentRefereeDialogMS"), QPoint(200, 850) },
    { QStringLiteral("IntelligentRefereeDialogFailed"),    QPoint(200, 850) },
    { QStringLiteral("RequestDrawDialog"),                 QPoint(200, 850) },
    { QStringLiteral("RequestRematchDialog"),              QPoint(200, 850) },
    { QStringLiteral("RequestResumeBattleDialog"),         QPoint(200, 850) },
    { QStringLiteral("RequestUndoDialog"),                 QPoint(200, 850) },
    { QStringLiteral("ReconnectSFDialog"),                 QPoint(200, 850) },
    { QStringLiteral("TipDialogWithButton"),               QPoint(490, 710) }
};

GameAnalyzer::GameAnalyzer(QObject *parent)
    : QObject{ parent },
      m_screencaptor{ nullptr },
      m_stop{ false },
      m_pauseDuration{ 0 },
      m_analyzeInterval{ 10 },
      m_running{ false },
      m_pauseGameAnalyze{ false },
      m_pauseBoardAnalyze{ false }
{
}

GameData::AppNavigation GameAnalyzer::appNavigationAnalyze(const QImage &image)
{
    auto funcEqual([image](const QString &name) -> bool
                   { return Util::isRegionEqual(image, name, templateImagePoints.value(name)); });

    if (funcEqual(QStringLiteral("TipDialogWithButton")))
    {
        if (funcEqual(QStringLiteral("RequestCountingDialog")) || funcEqual(QStringLiteral("RequestCountingDialogMS")))
            return GameData::requestCountingDialog;
        if (funcEqual(QStringLiteral("RequestDrawDialog")))
            return GameData::requestDrawDialog;
        if (funcEqual(QStringLiteral("RequestRematchDialog")))
            return GameData::requestRematchDialog;
        if (funcEqual(QStringLiteral("RequestResumeBattleDialog")))
            return GameData::requestResumeBattleDialog;
        if (funcEqual(QStringLiteral("RequestUndoDialog")))
            return GameData::requestUndoDialog;
        if (funcEqual(QStringLiteral("ConfirmDefeatDialog")) || funcEqual(QStringLiteral("ConfirmDefeatDialog2")))
            return GameData::confirmDefeatDialog;
        if (funcEqual(QStringLiteral("CancelResumeBattleDialog")))
            return GameData::cancelResumeBattleDialog;
        if (funcEqual(QStringLiteral("RequestIntelligentRefereeDialogMS")))
            return GameData::requestIntelligentRefereeDialog;
        if (funcEqual(QStringLiteral("IntelligentRefereeDialogFailed")))
            return GameData::intelligentRefereeDialogFailed;
        if (funcEqual(QStringLiteral("ReconnectSFDialog")))
            return GameData::ReconnectSuccessAndFinishDialog;
        const auto debugImageFilePath = Global::saveDebugImage(image);
        qWarning() << "dialog not matching" << debugImageFilePath;
        return GameData::otherDialog;
    }
    if (funcEqual(QStringLiteral("AcceptCountingResult")))
        return GameData::acceptCountingResultDialog;
    if (funcEqual(QStringLiteral("DurationChoiceDialog")))
        return GameData::durationChoiceDialog;
    if (funcEqual(QStringLiteral("MatchDialog")))
        return GameData::matchDialog;
    if (funcEqual(QStringLiteral("GameOverDialogCloseButton")) || funcEqual(QStringLiteral("GameOverDialogCloseButton2")))
        return GameData::gameOverDialog;
    if (funcEqual(QStringLiteral("LevelUpDialogCloseButton")))
        return GameData::levelUpDialog;
    if (funcEqual(QStringLiteral("PlayingPage")) || funcEqual(QStringLiteral("PlayingPage2")))
    {
        if (image.pixel(288, 946) == qRgb(80, 111, 78))
            return GameData::tipDialog;
        return GameData::playingPage;
    }
    if (funcEqual(QStringLiteral("PlayingPageWithMove")) || funcEqual(QStringLiteral("PlayingPageWithMove2")))
        return GameData::playingPageWithMove;
    if (funcEqual(QStringLiteral("AnalysisPage")) || funcEqual(QStringLiteral("AnalysisPage2")))
        return GameData::analysisPage;
    if (funcEqual(QStringLiteral("MainPage")))
        return GameData::mainPage;
    if (funcEqual(QStringLiteral("Backutton")) || funcEqual(QStringLiteral("Backutton2")) || funcEqual(QStringLiteral("Backutton3")))
        return GameData::pageWithBack;

    if (funcEqual(QStringLiteral("AIPKClose")))
        return GameData::AIPKDialog;

    qWarning() << "not matching" << Global::saveDebugImage(image);
    return GameData::unknownPage;
}

bool GameAnalyzer::isTurnToPlay(const QImage &image)
{
    const QRgb pixelValue = image.pixel(305, 305);
    return pixelValue == qRgb(255, 254, 118);
}

StoneData::StoneColor GameAnalyzer::getMyStoneColor(const QImage &image)
{
    const QColor pixelValue = image.pixel(150, 200);
    if (pixelValue.lightness() < 50)
        return StoneData::StoneColor::Black;
    else if (pixelValue.lightness() > 200)
        return StoneData::StoneColor::White;
    else
    {
        qWarning() << Q_FUNC_INFO
                   << QStringLiteral("myStoneColor check failed, rgb:")
                   << pixelValue.rgb()
                   << Global::saveDebugImage(image);
        return StoneData::StoneColor::None;
    }
}

void GameAnalyzer::getBoardArray(const QImage &image, BoardData &boardData)
{
    // WARNING:
    // 未处理悔棋情况
    // 当进入倒计时才识别时, 可能会出现lastMoveStone为空(影响GTP)
    // initialStonesArray与lastMoveStone和moveStonesArray冲突(影响Analysis)

    // 棋盘大小为19x19
    constexpr int rows = 19;
    constexpr int cols = 19;

    // 棋盘的四个已知角坐标
    constexpr QPointF topLeft(37, 361);        // 左上角
    constexpr QPointF bottomRight(1041, 1364); // 右下角

    // 计算每个格子的宽度和高度
    const double cellWidth = double(bottomRight.x() - topLeft.x()) / double(cols - 1);
    const double cellHeight = double(bottomRight.y() - topLeft.y()) / double(rows - 1);

    // 用于略微调整棋子检测点的位置，避免棋盘线干扰
    constexpr double offsetX = -10; // x轴偏移量
    constexpr double offsetY = -10; // y轴偏移量
    constexpr double offsetCX = 8;  // 监测点x轴偏移量
    constexpr double offsetCY = 8;  // 监测点y轴偏移量

    const auto needInitialStones(boardData.m_initialStonesArray.isEmpty() && boardData.m_moveStonesArray.isEmpty());

    // 遍历棋盘的每个格子，判断当前格子内是否有棋子
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            // 计算当前格子的中心坐标
            const double centerX = topLeft.x() + (double(j) * cellWidth);
            const double centerY = topLeft.y() + (double(i) * cellHeight);

            // 调整检测点的坐标，避免棋盘线影响
            const int checkX = qRound(centerX + offsetX);
            const int checkY = qRound(centerY + offsetY);

            // 偏左下15像素的点，用于进一步检测
            const int checkCX = qRound(centerX + offsetCX);
            const int checkCY = qRound(centerY + offsetCY);

            const QColor pixelValue = image.pixel(checkX, checkY);

            int currentPiece(0);

            // 根据检测到的棋子颜色绘制相应的圆点
            if (pixelValue.lightness() < 100) // 黑子
            {
                currentPiece = 1;
            }
            else if (pixelValue.lightness() > 240) // 白子
            {
                currentPiece = 2;
            }
            // 更新二维数组中的棋盘状态
            boardData.m_boardDataArray[i][j] = currentPiece;
            if (currentPiece == 0)
                continue;

            const QList<QColor> pixelCValues{ image.pixel(checkCX, checkCY),
                                              image.pixel(checkCX + 5, checkCY),
                                              image.pixel(checkCX, checkCY + 5) };
            const unsigned char lastMovePixelValue(currentPiece == 1 ? 255 : 0);
            const auto isLastMove(pixelCValues.at(0).lightness() == lastMovePixelValue && pixelCValues.at(1).lightness() == lastMovePixelValue && pixelCValues.at(2).lightness() == lastMovePixelValue);
            const StoneData::StoneColor lastMoveColor(currentPiece == 1 ? StoneData::StoneColor::Black : StoneData::StoneColor::White);
            if (isLastMove)
            {
                boardData.m_lastMoveStone.setColor(lastMoveColor);
                boardData.m_lastMoveStone.setPoint(QPoint(j, i));
                if (boardData.m_moveStonesArray.isEmpty() || boardData.m_moveStonesArray.last() != boardData.m_lastMoveStone)
                    boardData.m_moveStonesArray.append(boardData.m_lastMoveStone);
            }
            else if (needInitialStones)
            {
                boardData.m_initialStonesArray.append(StoneData(lastMoveColor, QPoint(j, i)));
            }
        }
    }
    // 己方执黑第一步时, needInitialStones会为true导致initialStonesArray异常
    if (needInitialStones && boardData.m_initialStonesArray.size() == 1)
    {
        boardData.m_moveStonesArray = boardData.m_initialStonesArray;
        boardData.m_initialStonesArray.clear();
    }
}
Screencaptor *GameAnalyzer::screencaptor() const
{
    return m_screencaptor;
}

void GameAnalyzer::setScreencaptor(Screencaptor *newScreencaptor)
{
    if (m_screencaptor == newScreencaptor)
        return;
    m_screencaptor = newScreencaptor;
    emit screencaptorChanged();
}

GameData GameAnalyzer::analyze()
{
    const auto image = screencap();
    if (!image)
    {
        qWarning() << QStringLiteral("image is null");
        return {};
    }
    return analyze(image.value());
}

GameData GameAnalyzer::analyze(const QImage &image)
{
    qDebug() << Q_FUNC_INFO;
    GameData gameData;
    handleImage(image, gameData);
    return gameData;
}

void GameAnalyzer::analyzeIndefinitely()
{
    qDebug() << Q_FUNC_INFO << "enter";
    if (m_running)
    {
        qWarning() << QStringLiteral("already running");
        return;
    }
    resetData();
    QTimer::singleShot(0, this, &GameAnalyzer::doAnalyze);
    qDebug() << Q_FUNC_INFO << "exit";
}

void GameAnalyzer::init()
{
    m_screencaptor->init();
}

void GameAnalyzer::reset()
{
    qDebug() << Q_FUNC_INFO;
    resetData();
    m_pauseDuration = 0;
    m_pauseGameAnalyze = false;
    m_pauseBoardAnalyze = false;
}

void GameAnalyzer::stopAnalyze()
{
    qDebug() << Q_FUNC_INFO;
    m_stop = true;
}

void GameAnalyzer::pause(const unsigned long &duration)
{
    setPauseDuration(duration);
}

bool GameAnalyzer::pauseBoardAnalyze() const
{
    return m_pauseBoardAnalyze;
}

void GameAnalyzer::setPauseBoardAnalyze(bool newPauseBoardAnalyze)
{
    if (m_pauseBoardAnalyze == newPauseBoardAnalyze)
        return;
    m_pauseBoardAnalyze = newPauseBoardAnalyze;
    emit pauseBoardAnalyzeChanged();
}

bool GameAnalyzer::pauseGameAnalyze() const
{
    return m_pauseGameAnalyze;
}

void GameAnalyzer::setPauseGameAnalyze(bool newPauseGameAnalyze)
{
    if (m_pauseGameAnalyze == newPauseGameAnalyze)
        return;
    m_pauseGameAnalyze = newPauseGameAnalyze;
    emit pauseGameAnalyzeChanged();
}

bool GameAnalyzer::running() const
{
    return m_running;
}

unsigned long GameAnalyzer::analyzeInterval() const
{
    return m_analyzeInterval;
}

void GameAnalyzer::setAnalyzeInterval(unsigned long newAnalyzeInterval)
{
    if (m_analyzeInterval == newAnalyzeInterval)
        return;
    m_analyzeInterval = newAnalyzeInterval;
    emit analyzeIntervalChanged();
}

unsigned long GameAnalyzer::pauseDuration() const
{
    return m_pauseDuration;
}

void GameAnalyzer::setPauseDuration(const unsigned long &newPauseDuration)
{
    if (m_pauseDuration == newPauseDuration)
        return;
    m_pauseDuration = newPauseDuration;
    emit pauseDurationChanged();
}

std::optional<QImage> GameAnalyzer::screencap()
{
    if (m_screencaptor == nullptr)
    {
        qWarning() << QStringLiteral("screencaptor is null");
        return {};
    }
    return m_screencaptor->screencap();
}

void GameAnalyzer::handleImage(const QImage &image, GameData &gameData)
{
    gameData.m_appNavigation = appNavigationAnalyze(image);
    if (gameData.m_appNavigation == GameData::playingPage)
    {
        if (m_pauseBoardAnalyze)
            gameData.m_boardData = m_data.m_boardData;
        else
            getBoardArray(image, gameData.m_boardData);
        gameData.m_myStoneColor = getMyStoneColor(image);
        gameData.m_boardData.m_myStoneColor = gameData.m_myStoneColor;
        gameData.m_needMove = isTurnToPlay(image);
    }
    emit analysisCompleted(gameData);
}

void GameAnalyzer::doAnalyze()
{
    if (m_pauseGameAnalyze)
    {
        QTimer::singleShot(m_analyzeInterval, this, &GameAnalyzer::doAnalyze);
        return;
    }
    if (m_stop)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("stopped");
        m_running = false;
        return;
    }
    if (m_pauseDuration > 0)
    {
        QTimer::singleShot(m_pauseDuration, this, &GameAnalyzer::doAnalyze);
        setPauseDuration(0);
        return;
    }
    m_running = true;
    auto image = screencap();
    if (!image)
    {
        qWarning() << QStringLiteral("image is null");
        m_running = false;
        return;
    }
    GameData gameData(m_data);
    if (m_stop)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("stopped");
        m_running = false;
        return;
    }
    handleImage(image.value(), gameData);
    if (m_stop)
    {
        qDebug() << Q_FUNC_INFO << QStringLiteral("stopped");
        m_running = false;
        return;
    }
    if (gameData != m_data)
    {
        m_data = gameData;
        emit analysisDataUpdated(gameData);
    }
    QTimer::singleShot(m_analyzeInterval, this, &GameAnalyzer::doAnalyze);
}

void GameAnalyzer::resetData()
{
    qDebug() << Q_FUNC_INFO;
    m_stop = false;
    m_data = {};
}
