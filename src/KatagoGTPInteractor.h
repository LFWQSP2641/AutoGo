#ifndef KATAGOGTPINTERACTOR_H
#define KATAGOGTPINTERACTOR_H

#include "KatagoInteractor.h"

class KatagoGTPInteractor : public KatagoInteractor
{
    Q_OBJECT

public:
    explicit KatagoGTPInteractor(QObject *parent = nullptr);

public slots:
    void init() override;
    void clearBoard() override;
    void stopAnalyze() override;
    void move(const BoardData &boardData) override;

    void startTimer();

protected:
    StoneData m_bestMove;
    QTimer *timer;

protected slots:
    void analyzeKatagoOutput() override;

    void emitBestMove();
};

#endif // KATAGOGTPINTERACTOR_H
