#include "AdbInteractor.h"

#include "Settings.h"

AdbInteractor::AdbInteractor()
    : m_adbHandle{ AdbHandle::instance() }
{
}

bool AdbInteractor::adbConnect()
{
    if (m_adbHandle->isAdbConnected())
    {
        return true;
    }
    m_adbHandle->m_adbPath = Settings::getSingletonSettings()->adbPath();
    m_adbHandle->m_adbSerial = Settings::getSingletonSettings()->adbSerial();
    return m_adbHandle->init();
}
