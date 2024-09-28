#ifndef KATAGOINTERACTOR_H
#define KATAGOINTERACTOR_H

#include "BoardData.h"

#include <QObject>

class QProcess;
class QEventLoop;

class KatagoInteractor : public QObject
{
    Q_OBJECT

public:
    explicit KatagoInteractor(QObject *parent = nullptr);

public slots:
    void init();
    void clearBoard();
    void stopAnalyze();
    void move(const BoardData &boardData);

protected:
    BoardData m_boardData;

    QProcess *katagoProcess;
    QEventLoop *eventLoop;
    QTimer *timeOut;
    QByteArray bytes;

    static QString pointToGTP(const QPoint &point);
    static QPoint gptToPoint(const QString &gtpMove);

    static QJsonArray stoneDataListToJsonArray(const QList<StoneData> &stoneDataList);

protected slots:
    void analyzeKatagoOutput();

signals:
    void initFinished(bool success);
    void bestMove(const StoneData &stoneData);
};

#endif // KATAGOINTERACTOR_H
