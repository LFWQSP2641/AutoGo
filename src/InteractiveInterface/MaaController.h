#ifndef MAACONTROLLER_H
#define MAACONTROLLER_H

#include "Controller.h"

class QProcess;

class MaaController : public Controller
{
    Q_OBJECT

public:
    explicit MaaController(QObject *parent = nullptr);

    virtual bool init() override;

    virtual bool click(const QPoint &p) override;

protected:
    QProcess *maaTouchProcess;
};

#endif // MAACONTROLLER_H
