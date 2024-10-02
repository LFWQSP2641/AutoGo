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

    // 对局模式时长
    enum TimeMode
    {
        Short = 0,
        Medium,
        Long
    };
    Q_ENUM(TimeMode)

    KatagoInteractor::TimeMode timeMode() const;
    void setTimeMode(KatagoInteractor::TimeMode newTimeMode);

    int getReportIntervalMS() const;
    void setReportIntervalMS(int newReportIntervalMS);

    qint64 getMinMoveInterval() const;
    void setMinMoveInterval(qint64 newMinMoveInterval);

public slots:
    virtual void init();
    virtual void clearBoard();
    virtual void stopAnalyze() = 0;
    virtual void move(const BoardData &boardData) = 0;

    virtual void startTimer();

    void setTimeModeFromInt(int newTimeMode);

protected:
    BoardData m_boardData;
    KatagoInteractor::TimeMode m_timeMode;

    QProcess *katagoProcess;
    QByteArray bytes;

    int reportIntervalMS = 1000; // ms
    StoneData m_bestMove;
    QTimer *timer;

    qint64 lastMoveTime = 0;
    qint64 minMoveInterval = 2000; // ms

    virtual QStringList getKataGoArgs() const = 0;

    static QString pointToGTP(const QPoint &point);
    static QPoint gptToPoint(const QString &gtpMove);

    static QJsonArray stoneDataListToJsonArray(const QList<StoneData> &stoneDataList);

protected slots:
    void emitBestMove();
    virtual void analyzeKatagoOutput() = 0;
    virtual void analyzeKatagoInit() = 0;

signals:
    void initFinished(bool success);
    void bestMove(const StoneData &stoneData);
    void timeModeChanged();

    // GUI
    void bestMoveUpdate(const StoneData &stoneData);

    void reportIntervalMSChanged();

    void minMoveIntervalChanged();

private:
    Q_PROPERTY(KatagoInteractor::TimeMode timeMode READ timeMode WRITE setTimeMode NOTIFY timeModeChanged FINAL)
    Q_PROPERTY(int reportIntervalMS READ getReportIntervalMS WRITE setReportIntervalMS NOTIFY reportIntervalMSChanged FINAL)
    Q_PROPERTY(qint64 minMoveInterval READ getMinMoveInterval WRITE setMinMoveInterval NOTIFY minMoveIntervalChanged FINAL)
};

#endif // KATAGOINTERACTOR_H
