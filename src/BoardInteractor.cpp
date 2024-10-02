#include "BoardInteractor.h"

#include "InteractiveInterface/MaaController.h"

#include <QDebug>
#include <QRect>
#include <QTimer>

BoardInteractor::BoardInteractor(QObject *parent)
    : QObject{ parent },
      controller(new MaaController(this)),
      m_timeMode(BoardInteractor::TimeMode::Short)
{
}

void BoardInteractor::init()
{
    controller->init();
}

void BoardInteractor::setTimeModeFromInt(int newTimeMode)
{
    qDebug() << newTimeMode;
    setTimeMode(BoardInteractor::TimeMode(newTimeMode));
}

void BoardInteractor::moveStone(const StoneData &stoneData)
{
    moveStone(stoneData.getPoint());
}

void BoardInteractor::moveStone(const QPoint &stonePoint)
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
    QTimer::singleShot(100, this, &BoardInteractor::clickConfirmMove);
    QTimer::singleShot(200, this, &BoardInteractor::moveFinished);
}

void BoardInteractor::acceptRequest()
{
    controller->Controller::click(QRect(QPoint(604, 1089), QPoint(939, 1172)));
}

void BoardInteractor::rejectRequest()
{
    controller->Controller::click(QRect(QPoint(142, 1089), QPoint(475, 1172)));
}

void BoardInteractor::closeRequest()
{
    controller->Controller::click(QRect(QPoint(372, 1089), QPoint(706, 1172)));
}

void BoardInteractor::matchGame()
{
    matchGame1();
    QTimer::singleShot(100, this, &BoardInteractor::matchGame2);
}

void BoardInteractor::closeGameOverDialog()
{
    controller->Controller::click(QRect(QPoint(975, 328), QPoint(1020, 378)));
}

void BoardInteractor::closeLevelUpDialog()
{
    controller->Controller::click(QRect(QPoint(935, 205), QPoint(980, 260)));
}

void BoardInteractor::backToMain()
{
    controller->Controller::click(QRect(QPoint(40, 125), QPoint(100, 150)));
}

BoardInteractor::TimeMode BoardInteractor::timeMode() const
{
    return m_timeMode;
}

void BoardInteractor::setTimeMode(BoardInteractor::TimeMode newTimeMode)
{
    if (m_timeMode == newTimeMode)
        return;
    m_timeMode = newTimeMode;
    emit timeModeChanged();
}

void BoardInteractor::matchGame1()
{
    controller->Controller::click(QRect(QPoint(34, 1357), QPoint(338, 1514)));
}

void BoardInteractor::matchGame2()
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

void BoardInteractor::clickConfirmMove()
{
    controller->Controller::click(QRect(QPoint(378, 1595), QPoint(700, 1767)));
}
