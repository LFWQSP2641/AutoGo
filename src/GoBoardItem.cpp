#include "GoBoardItem.h"

GoBoardItem::GoBoardItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    // 初始化19x19的棋盘，0表示无子
    m_boardData = QVector<QVector<int>>(19, QVector<int>(19, 0));
}

void GoBoardItem::paint(QPainter *painter)
{
    int gridSize = qMin(width(), height()) / 20; // 格子大小
    int margin = gridSize;                       // 边界大小

    // 绘制棋盘线条
    painter->setPen(QPen(Qt::black, 2));
    for (int i = 0; i < 19; ++i)
    {
        // 横线
        painter->drawLine(margin, margin + i * gridSize, margin + 18 * gridSize, margin + i * gridSize);
        // 纵线
        painter->drawLine(margin + i * gridSize, margin, margin + i * gridSize, margin + 18 * gridSize);
    }

    // 绘制星位
    drawStarPoints(painter);

    // 绘制棋子
    for (int row = 0; row < 19; ++row)
    {
        for (int col = 0; col < 19; ++col)
        {
            int stone = m_boardData[row][col];
            if (stone != 0)
            {
                painter->setBrush(stone == 1 ? Qt::black : Qt::white);
                painter->setPen(QPen(Qt::black, 2)); // 设置边界为黑色
                int x = margin + col * gridSize;
                int y = margin + row * gridSize;
                painter->drawEllipse(QPointF(x, y), gridSize / 2.5, gridSize / 2.5); // 绘制圆形棋子
            }
        }
    }

    drawBestMove(painter, m_bestMove);
}

void GoBoardItem::setBoardData(const QVector<QVector<int>> &data)
{
    m_boardData = data;
    update(); // 触发重绘
}

void GoBoardItem::setBestMove(const StoneData &newBestMove)
{
    m_bestMove = newBestMove;
    update(); // 触发重绘
}

void GoBoardItem::drawStarPoints(QPainter *painter)
{
    int gridSize = qMin(width(), height()) / 20; // 每格的实际大小
    int margin = gridSize;                       // 边距大小
    int starRadius = gridSize / 5;               // 增加星位半径，使其更大

    // 星位位置 (19x19 棋盘上的标准星位)
    QList<QPoint> starPoints = {
        QPoint(3, 3), QPoint(3, 9), QPoint(3, 15), // 调整后的星位位置
        QPoint(9, 3), QPoint(9, 9), QPoint(9, 15),
        QPoint(15, 3), QPoint(15, 9), QPoint(15, 15)
    };

    painter->setBrush(QBrush(Qt::black)); // 星位颜色
    painter->setPen(Qt::NoPen);           // 不需要边框

    // 计算星位的坐标
    for (const QPoint &point : starPoints)
    {
        int x = margin + point.x() * gridSize;
        int y = margin + point.y() * gridSize;
        painter->drawEllipse(QPointF(x, y), starRadius, starRadius); // 绘制星位
    }
}

void GoBoardItem::drawBestMove(QPainter *painter, const StoneData &bestMove)
{
    if (bestMove.getColor() == StoneData::None)
    {
        return; // 如果没有最佳下法，不绘制
    }

    int gridSize = qMin(width(), height()) / 20; // 格子大小
    int margin = gridSize;                       // 边界大小

    // 获取最佳下法的位置
    int x = margin + bestMove.getPoint().x() * gridSize;
    int y = margin + bestMove.getPoint().y() * gridSize;

    // 绘制外层蓝色圆圈
    painter->setPen(QPen(Qt::blue, 3)); // 蓝色边框
    painter->setBrush(Qt::NoBrush);     // 不填充内部
    painter->drawEllipse(QPointF(x, y), gridSize / 2, gridSize / 2);

    // 根据棋子的颜色绘制棋子
    painter->setPen(QPen(Qt::black, 2)); // 设置边界为黑色
    if (bestMove.getColor() == StoneData::Black)
    {
        painter->setBrush(Qt::black);
    }
    else if (bestMove.getColor() == StoneData::White)
    {
        painter->setBrush(Qt::white);
    }

    painter->drawEllipse(QPointF(x, y), gridSize / 2.5, gridSize / 2.5); // 绘制圆形棋子
}
