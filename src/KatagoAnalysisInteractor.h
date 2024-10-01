#ifndef KATAGOANALYSISINTERACTOR_H
#define KATAGOANALYSISINTERACTOR_H

#include "KatagoInteractor.h"

class KatagoAnalysisInteractor : public KatagoInteractor
{
    Q_OBJECT

public:
    explicit KatagoAnalysisInteractor(QObject *parent = nullptr);

public slots:
    virtual void clearBoard() override;
    virtual void stopAnalyze() override;
    virtual void move(const BoardData &boardData) override;

protected:
    virtual QStringList getKataGoArgs() const override;

protected slots:
    virtual void analyzeKatagoOutput() override;
    virtual void analyzeKatagoInit() override;
};

#endif // KATAGOANALYSISINTERACTOR_H
