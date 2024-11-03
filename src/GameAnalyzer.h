#ifndef GAMEANALYZER_H
#define GAMEANALYZER_H

#include "GameData.h"
#include "InteractiveInterface/Screencaptor.h"

class BoardAnalyzer;

class GameAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit GameAnalyzer(QObject *parent = nullptr);

    static QHash<QString, QPoint> templateImagePoints;

    static GameData::AppNavigation appNavigationAnalyze(const cv::Mat &image);

    static bool isTurnToPlay(const cv::Mat &image);
    static StoneData::StoneColor getMyStoneColor(const cv::Mat &image);

    static void getBoardArray(const cv::Mat &image, BoardData &boardData);

    Screencaptor *screencaptor() const;
    void setScreencaptor(Screencaptor *newScreencaptor);

    unsigned long pauseDuration() const;
    void setPauseDuration(const unsigned long &newPauseDuration);

    unsigned long analyzeInterval() const;
    void setAnalyzeInterval(unsigned long newAnalyzeInterval);

    bool running() const;

public slots:
    GameData analyze();
    GameData analyze(const cv::Mat &image);
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

    unsigned long m_pauseDuration;
    unsigned long m_analyzeInterval;

    bool m_running;

protected:
    std::optional<cv::Mat> screencap();

protected slots:
    void handleImage(const cv::Mat &image, GameData &gameData);

    void doAnalyze();

signals:
    void screencaptorChanged();

    void pauseDurationChanged();

    void analyzeIntervalChanged();

private:
    Q_PROPERTY(Screencaptor *screencaptor READ screencaptor WRITE setScreencaptor NOTIFY screencaptorChanged FINAL)
    Q_PROPERTY(unsigned long pauseDuration READ pauseDuration WRITE setPauseDuration NOTIFY pauseDurationChanged FINAL)
    Q_PROPERTY(unsigned long analyzeInterval READ analyzeInterval WRITE setAnalyzeInterval NOTIFY analyzeIntervalChanged FINAL)
    Q_PROPERTY(bool running READ running CONSTANT FINAL)
};

#endif // GAMEANALYZER_H
