#ifndef BOARDINTERACTOR_H
#define BOARDINTERACTOR_H

#include "StoneData.h"

#include <QObject>

class MaaController;

class BoardInteractor : public QObject
{
    Q_OBJECT

public:
    explicit BoardInteractor(QObject *parent = nullptr);

    // 对局模式时长
    enum TimeMode
    {
        Short = 0,
        Medium,
        Long
    };
    Q_ENUM(TimeMode)

    BoardInteractor::TimeMode timeMode() const;
    void setTimeMode(BoardInteractor::TimeMode newTimeMode);

public slots:
    void init();

    void setTimeModeFromInt(int newTimeMode);

    void moveStone(const StoneData &stoneData);
    void moveStone(const QPoint &stonePoint);

    void acceptRequest();
    void rejectRequest();
    void closeRequest();

    void matchGame();

    void closeGameOverDialog();
    void closeLevelUpDialog();

    void backToMain();

protected:
    MaaController *controller;
    BoardInteractor::TimeMode m_timeMode;

protected slots:
    void matchGame1();
    void matchGame2();
    void clickConfirmMove();

signals:
    void moveFinished();
    void timeModeChanged();

private:
    Q_PROPERTY(BoardInteractor::TimeMode timeMode READ timeMode WRITE setTimeMode NOTIFY timeModeChanged FINAL)
};

#endif // BOARDINTERACTOR_H
