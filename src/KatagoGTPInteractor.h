#ifndef KATAGOGTPINTERACTOR_H
#define KATAGOGTPINTERACTOR_H

#include "KatagoInteractor.h"

class KatagoGTPInteractor : public KatagoInteractor
{
    Q_OBJECT

public:
    explicit KatagoGTPInteractor(QObject *parent = nullptr);

public slots:
    virtual void clearBoard() override;
    virtual void stopAnalyze() override;
    virtual void move(const BoardData &boardData) override;

    virtual void clearCache() override;

protected:
    virtual QStringList getKataGoArgs() const override;

protected slots:
    virtual void analyzeKatagoOutput() override;
    virtual void analyzeKatagoInit() override;
};

#endif // KATAGOGTPINTERACTOR_H
