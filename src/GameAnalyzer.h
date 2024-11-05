#ifndef GAMEANALYZER_H
#define GAMEANALYZER_H

#include "GameData.h"
#include "InteractiveInterface/Screencaptor.h"

class GameAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit GameAnalyzer(QObject *parent = nullptr);

    static QHash<QString, QPoint> templateImagePoints;

    static GameData::AppNavigation appNavigationAnalyze(const QImage &image);

    static bool isTurnToPlay(const QImage &image);
    static StoneData::StoneColor getMyStoneColor(const QImage &image);

    static void getBoardArray(const QImage &image, BoardData &boardData);

    Screencaptor *screencaptor() const;
    void setScreencaptor(Screencaptor *newScreencaptor);

    unsigned long pauseDuration() const;
    void setPauseDuration(const unsigned long &newPauseDuration);

    unsigned long analyzeInterval() const;
    void setAnalyzeInterval(unsigned long newAnalyzeInterval);

    bool running() const;

    bool pauseGameAnalyze() const;
    void setPauseGameAnalyze(bool newPauseGameAnalyze);

    bool pauseBoardAnalyze() const;
    void setPauseBoardAnalyze(bool newPauseBoardAnalyze);

public slots:
    GameData analyze();
    GameData analyze(const QImage &image);
    void analyzeIndefinitely();
    void init();
    void reset();
    void stopAnalyze();

    void pause(const unsigned long &duration);

signals:
    void analysisCompleted(const GameData &data);
    void analysisDataUpdated(const GameData &data);

protected:
    GameData m_data;

    Screencaptor *m_screencaptor;
    volatile bool m_stop;

    volatile unsigned long m_pauseDuration;
    unsigned long m_analyzeInterval;

    bool m_running;

    volatile bool m_pauseGameAnalyze;
    volatile bool m_pauseBoardAnalyze;

protected:
    std::optional<QImage> screencap();

protected slots:
    void handleImage(const QImage &image, GameData &gameData);

    void doAnalyze();

    void resetData();

signals:
    void screencaptorChanged();

    void pauseDurationChanged();

    void analyzeIntervalChanged();

    void pauseGameAnalyzeChanged();

    void pauseBoardAnalyzeChanged();

private:
    Q_PROPERTY(Screencaptor *screencaptor READ screencaptor WRITE setScreencaptor NOTIFY screencaptorChanged FINAL)
    Q_PROPERTY(unsigned long pauseDuration READ pauseDuration WRITE setPauseDuration NOTIFY pauseDurationChanged FINAL)
    Q_PROPERTY(unsigned long analyzeInterval READ analyzeInterval WRITE setAnalyzeInterval NOTIFY analyzeIntervalChanged FINAL)
    Q_PROPERTY(bool running READ running CONSTANT FINAL)
    Q_PROPERTY(bool pauseGameAnalyze READ pauseGameAnalyze WRITE setPauseGameAnalyze NOTIFY pauseGameAnalyzeChanged FINAL)
    Q_PROPERTY(bool pauseBoardAnalyze READ pauseBoardAnalyze WRITE setPauseBoardAnalyze NOTIFY pauseBoardAnalyzeChanged FINAL)
};

#endif // GAMEANALYZER_H
