#ifndef ADBINTERACTOR_H
#define ADBINTERACTOR_H

#include "InteractiveInterface/AdbHandle.h"

#include <QObject>

class AdbInteractor
{
public:
    AdbInteractor();

protected:
    bool adbConnect();

protected:
    AdbHandle *m_adbHandle;
};

#endif // ADBINTERACTOR_H
