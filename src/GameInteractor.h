#ifndef BOARDINTERACTOR_H
#define BOARDINTERACTOR_H

#include "InteractiveInterface/Controller.h"
#include "StoneData.h"

#include <QObject>

class GameInteractor : public QObject
{
    Q_OBJECT

public:
    explicit GameInteractor(QObject *parent = nullptr);

    // 对局模式时长
    enum TimeMode
    {
        Short = 0,
        Medium,
        Long
    };
    Q_ENUM(TimeMode)

    GameInteractor::TimeMode timeMode() const;
    void setTimeMode(GameInteractor::TimeMode newTimeMode);

    Controller *getController() const;
    void setController(Controller *newController);

public slots:
    void init();

    void setTimeModeFromInt(int newTimeMode);

    void moveStone(const StoneData &stoneData);
    void moveStone(const QPoint &stonePoint);

    void clickConfirmMove();

    void acceptRequest();
    void rejectRequest();
    void closeRequest();
    void acceptCountingResult();

    void matchGame();

    void openMatchDialog();
    void choiceGameMode();

    void closeGameOverDialog();
    void closeLevelUpDialog();

    void closeAIPKDialog();

    void backToMain();

    void requestCounting();
    void requestIntelligentReferee();

protected:
    Controller *controller;
    GameInteractor::TimeMode m_timeMode;

signals:
    void timeModeChanged();

    void controllerChanged();

private:
    Q_PROPERTY(GameInteractor::TimeMode timeMode READ timeMode WRITE setTimeMode NOTIFY timeModeChanged FINAL)
    Q_PROPERTY(Controller *controller READ getController WRITE setController NOTIFY controllerChanged FINAL)
};

#endif // BOARDINTERACTOR_H
