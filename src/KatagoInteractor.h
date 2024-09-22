#ifndef KATAGOINTERACTOR_H
#define KATAGOINTERACTOR_H

#include "StoneData.h"

#include <QObject>

class QProcess;
class QEventLoop;

class KatagoInteractor : public QObject
{
    Q_OBJECT

public:
    explicit KatagoInteractor(QObject *parent = nullptr);

    StoneData getLastMoveStone() const;

    StoneData::StoneColor getMyStoneColor() const;
    void setMyStoneColor(StoneData::StoneColor newMyStoneColor);

public slots:
    void init();
    void clearBoard();
    void stopAnalyze();
    void move(const StoneData &stoneData);

    StoneData getBestMove();

protected:
    StoneData::StoneColor myStoneColor;

    StoneData lastMoveStone;

    StoneData m_bestMove;

    QProcess *katagoProcess;
    QEventLoop *eventLoop;
    QTimer *timeOut;
    QByteArray bytes;

    void setBestMove(const StoneData &newBestMove);

    static QString pointToGTP(const QPoint &point);
    static QPoint gptToPoint(const QString &gtpMove);

protected slots:
    void analyzeKatagoOutput();

signals:
    void initFinished(bool success);
    // getBestMove调用时发出
    void bestMove(const StoneData &stoneData);
    // bestMove更改时发出
    void bestMoveChanged(const StoneData &stoneData);

    void myStoneColorChanged();

private:
    Q_PROPERTY(StoneData lastMoveStone READ getLastMoveStone CONSTANT FINAL)
    Q_PROPERTY(StoneData::StoneColor myStoneColor READ getMyStoneColor WRITE setMyStoneColor NOTIFY myStoneColorChanged FINAL)
    Q_PROPERTY(StoneData m_bestMove READ getBestMove WRITE setBestMove NOTIFY bestMoveChanged FINAL)
};

#endif // KATAGOINTERACTOR_H
