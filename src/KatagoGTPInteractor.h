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

protected:
    StoneData m_bestMove;

protected slots:
    void analyzeKatagoOutput() override;
};

#endif // KATAGOGTPINTERACTOR_H
