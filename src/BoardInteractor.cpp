#include "BoardInteractor.h"

#include "InteractiveInterface/MaaController.h"

#include <QRect>
#include <QTimer>

BoardInteractor::BoardInteractor(QObject *parent)
    : QObject{ parent },
      controller(new MaaController(this))
{
}

void BoardInteractor::init()
{
    controller->init(QStringLiteral("127.0.0.1:16416"));
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

void BoardInteractor::startGame()
{
    startGame1();
    QTimer::singleShot(100, this, &BoardInteractor::startGame2);
    QTimer::singleShot(200, this, &BoardInteractor::startGameFinished);
}

void BoardInteractor::startGame1()
{
    controller->Controller::click(QRect(QPoint(34, 1357), QPoint(338, 1514)));
}

void BoardInteractor::startGame2()
{
    controller->Controller::click(QRect(QPoint(175, 646), QPoint(900, 848)));
}

void BoardInteractor::clickConfirmMove()
{
    controller->Controller::click(QRect(QPoint(378, 1595), QPoint(700, 1767)));
}
