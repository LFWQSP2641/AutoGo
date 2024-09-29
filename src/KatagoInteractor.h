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

public slots:
    virtual void init() = 0;
    virtual void clearBoard();
    virtual void stopAnalyze() = 0;
    virtual void move(const BoardData &boardData) = 0;

    void setTimeModeFromInt(int newTimeMode);

protected:
    BoardData m_boardData;
    KatagoInteractor::TimeMode m_timeMode;

    QProcess *katagoProcess;
    QEventLoop *eventLoop;
    QTimer *timeOut;
    QByteArray bytes;

    static QString pointToGTP(const QPoint &point);
    static QPoint gptToPoint(const QString &gtpMove);

    static QJsonArray stoneDataListToJsonArray(const QList<StoneData> &stoneDataList);

protected slots:
    virtual void analyzeKatagoOutput() = 0;

signals:
    void initFinished(bool success);
    void bestMove(const StoneData &stoneData);
    void timeModeChanged();

private:
    Q_PROPERTY(KatagoInteractor::TimeMode timeMode READ timeMode WRITE setTimeMode NOTIFY timeModeChanged FINAL)
};

#endif // KATAGOINTERACTOR_H
