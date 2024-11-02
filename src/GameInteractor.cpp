#include "GameInteractor.h"

#include "InteractiveInterface/Controller.h"

#include <QDebug>
#include <QRect>
#include <QTimer>

GameInteractor::GameInteractor(QObject *parent)
    : QObject{ parent },
      controller(nullptr),
      m_timeMode(GameInteractor::TimeMode::Short)
{
}

void GameInteractor::init()
{
    controller->init();
}

void GameInteractor::setTimeModeFromInt(int newTimeMode)
{
    qDebug() << newTimeMode;
    setTimeMode(GameInteractor::TimeMode(newTimeMode));
}

void GameInteractor::moveStone(const StoneData &stoneData)
{
    moveStone(stoneData.getPoint());
}

void GameInteractor::moveStone(const QPoint &stonePoint)
{
    // 棋盘大小为19x19
    const int rows = 19;
    const int cols = 19;

    // 棋盘的四个已知角坐标
    const QPoint topLeft(37, 361);        // 左上角
    const QPoint bottomRight(1041, 1365); // 右下角

    // 计算每个格子的宽度和高度
    const float cellWidth = (bottomRight.x() - topLeft.x()) / (cols - 1);
    const float cellHeight = (bottomRight.y() - topLeft.y()) / (rows - 1);

    const int centerX = static_cast<int>(topLeft.x() + stonePoint.x() * cellWidth);
    const int centerY = static_cast<int>(topLeft.y() + stonePoint.y() * cellHeight);

    const QPoint centerPoint(centerX, centerY);

    // 减少矩形的宽度和高度
    const int rectWidth = 8;
    const int rectHeight = 8;

    // 以中心点为基准，计算矩形的左上角坐标
    const int rectX = centerPoint.x() - rectWidth / 2;
    const int rectY = centerPoint.y() - rectHeight / 2;

    controller->Controller::click(QRect(rectX, rectY, rectWidth, rectHeight));
    QTimer::singleShot(100, this, &GameInteractor::clickConfirmMove);
    QTimer::singleShot(200, this, &GameInteractor::moveFinished);
}

void GameInteractor::acceptRequest()
{
    controller->Controller::click(QRect(QPoint(604, 1089), QPoint(939, 1172)));
}

void GameInteractor::rejectRequest()
{
    controller->Controller::click(QRect(QPoint(142, 1089), QPoint(475, 1172)));
}

void GameInteractor::closeRequest()
{
    controller->Controller::click(QRect(QPoint(372, 1089), QPoint(706, 1172)));
}

void GameInteractor::acceptCountingResult()
{
    controller->Controller::click(QRect(QPoint(603, 1780), QPoint(938, 1863)));
}

void GameInteractor::matchGame()
{
    openMatchDialog();
    QTimer::singleShot(100, this, &GameInteractor::closeGameOverDialog);
}

void GameInteractor::openMatchDialog()
{
    controller->Controller::click(QRect(QPoint(34, 1357), QPoint(338, 1514)));
}

void GameInteractor::choiceGameMode()
{
    switch (m_timeMode)
    {
    case 0:
        controller->Controller::click(QRect(QPoint(175, 646), QPoint(900, 848)));
        break;
    case 1:
        controller->Controller::click(QRect(QPoint(175, 905), QPoint(900, 1107)));
        break;
    case 2:
        controller->Controller::click(QRect(QPoint(175, 1164), QPoint(900, 1366)));
        break;
    default:
        break;
    }
}

void GameInteractor::closeGameOverDialog()
{
    controller->Controller::click(QRect(QPoint(975, 328), QPoint(1020, 378)));
}

void GameInteractor::closeLevelUpDialog()
{
    controller->Controller::click(QRect(QPoint(935, 205), QPoint(980, 260)));
}

void GameInteractor::backToMain()
{
    controller->Controller::click(QRect(QPoint(40, 125), QPoint(100, 150)));
}

Controller *GameInteractor::getController() const
{
    return controller;
}

void GameInteractor::setController(Controller *newController)
{
    if (controller == newController)
        return;
    controller = newController;
    emit controllerChanged();
}

GameInteractor::TimeMode GameInteractor::timeMode() const
{
    return m_timeMode;
}

void GameInteractor::setTimeMode(GameInteractor::TimeMode newTimeMode)
{
    if (m_timeMode == newTimeMode)
        return;
    m_timeMode = newTimeMode;
    emit timeModeChanged();
}

void GameInteractor::clickConfirmMove()
{
    controller->Controller::click(QRect(QPoint(378, 1595), QPoint(700, 1767)));
}
