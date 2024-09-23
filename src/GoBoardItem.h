#ifndef GOBOARDITEM_H
#define GOBOARDITEM_H

#include "StoneData.h"

#include <QPainter>
#include <QQmlEngine>
#include <QQuickPaintedItem>
#include <QtQml/qqmlregistration.h>

class GoBoardItem : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit GoBoardItem(QQuickItem *parent = nullptr);

    // 重写paint方法，自定义绘制逻辑
    void paint(QPainter *painter) override;

public slots:
    // 设置棋盘数据，1表示黑子，2表示白子
    void setBoardData(const QVector<QVector<int>> &data);
    void setBestMove(const StoneData &newBestMove);

    void clearBoardData();
    void clearBestMove();

private:
    QVector<QVector<int>> m_boardData;

    StoneData m_bestMove;

private slots:
    void drawStarPoints(QPainter *painter);
    void drawBestMove(QPainter *painter, const StoneData &bestMove); // 绘制最佳下法
};

#endif // GOBOARDITEM_H
