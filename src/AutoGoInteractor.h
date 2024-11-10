#ifndef AUTOGOINTERACTOR_H
#define AUTOGOINTERACTOR_H

#include "GameBoardHandler.h"

#include <QObject>
#include <QtQml/qqmlregistration.h>

class AutoGoInteractor : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit AutoGoInteractor(QObject *parent = nullptr);
    ~AutoGoInteractor();

    GameBoardHandler *gameBoardHandler() const;

protected:
    GameBoardHandler *m_gameBoardHandler;
    QThread *m_gameBoardHandlerThread;

signals:
    void init();
    void startGame();
    void stopGame();
    void continuousStartGame();
    void setTimeMode(int timeMode);
    void clearKataGoCache();

signals:
    void boardDataArrayUpdate(const QList<QList<int>> boardDataArray);
    void bestPointUpdate(const StoneData &stoneData);

    void initStarting();

    void clearBestPoint();

    void gameStarting();
    void gameOver();

    void startInitFinished(bool success);

    void checkingAppNavigation();

    void logMessage(const QString &message, bool formatted = false);
    void errorOccurred(const QString &errorString, bool formatted = false);

signals:
    void taskChanged();
    void continuousPlayGameChanged();

private:
    Q_PROPERTY(GameBoardHandler *gameBoardHandler READ gameBoardHandler CONSTANT FINAL)
};

#endif // AUTOGOINTERACTOR_H
