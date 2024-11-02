#include "MumuHandle.h"

#include <QDir>
#include <QFile>
#include <QLibrary>

MumuHandle *MumuHandle::instance = nullptr;

MumuHandle *MumuHandle::getInstance()
{
    if (!instance)
    {
        instance = new MumuHandle();
    }
    return instance;
}

bool MumuHandle::init()
{
    if (inited)
    {
        return true;
    }

    QString dllPath(m_mumuPath);
    dllPath.append(QStringLiteral("/shell/sdk/external_renderer_ipc.dll"));
    if (!QFile(dllPath).exists())
        return false;
    library->setFileName(dllPath);
    typedef int (*NemuConnectType)(const wchar_t *, int);
    NemuConnectType nemuConnect = NemuConnectType(library->resolve("nemu_connect"));
    if (nemuConnect == nullptr)
        return false;
    std::wstring wstring(QDir::toNativeSeparators(m_mumuPath).toStdWString());
    m_mumuHandle = nemuConnect(wstring.c_str(), m_mumuInstIndex);
    if (m_mumuHandle == 0)
        return false;

    m_nemuCaptureDisplayFunction =
        NemuCaptureDisplayType(library->resolve("nemu_capture_display"));
    if (m_nemuCaptureDisplayFunction == nullptr)
        return false;

    int ret = m_nemuCaptureDisplayFunction(
        m_mumuHandle,
        m_mumuDisplayId,
        0,
        &m_displayWidth,
        &m_displayHeight,
        nullptr);
    if (ret != 0)
    {
        uninit();
        return false;
    }

    m_displayBufferSize = m_displayWidth * m_displayHeight * 4;

    return true;
}

void MumuHandle::uninit()
{
    library->unload();
}

bool MumuHandle::isInit() const
{
    return inited;
}

MumuHandle::MumuHandle()
    : library(new QLibrary)
{
}

MumuHandle::~MumuHandle()
{
    library->deleteLater();
}
