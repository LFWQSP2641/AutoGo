#ifndef KATAGOANALYSISINTERACTOR_H
#define KATAGOANALYSISINTERACTOR_H

#include "KatagoInteractor.h"

class KatagoAnalysisInteractor : public KatagoInteractor
{
    Q_OBJECT

public:
    explicit KatagoAnalysisInteractor(QObject *parent = nullptr);

public slots:
    void init() override;
    void clearBoard() override;
    void stopAnalyze() override;
    void move(const BoardData &boardData) override;

protected slots:
    void analyzeKatagoOutput() override;
};

#endif // KATAGOANALYSISINTERACTOR_H
