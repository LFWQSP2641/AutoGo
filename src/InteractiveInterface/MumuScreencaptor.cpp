#include "MumuScreencaptor.h"

#include "MumuHandle.h"
#include "src/Settings.h"

#include <QDir>
#include <QFile>
#include <QLibrary>

MumuScreencaptor::MumuScreencaptor(QObject *parent)
    : Screencaptor{ parent },
      m_mumuHandle{ MumuHandle::getInstance() }
{
}

std::optional<QImage> MumuScreencaptor::screencap()
{
    if (m_mumuHandle->m_nemuCaptureDisplayFunction == nullptr)
        return std::nullopt;
    int ret = m_mumuHandle->m_nemuCaptureDisplayFunction(
        m_mumuHandle->m_mumuHandle,
        Settings::getSingletonSettings()->mumuDisplayId(),
        static_cast<int>(m_displayBuffer.size()),
        &m_mumuHandle->m_displayWidth,
        &m_mumuHandle->m_displayHeight,
        m_displayBuffer.data());
    if (ret)
        return std::nullopt;

    QImage img(m_displayBuffer.data(), m_mumuHandle->m_displayWidth, m_mumuHandle->m_displayHeight, QImage::Format_RGBA8888);
    img.mirror();

    return img;
}

bool MumuScreencaptor::init()
{
    if (m_mumuHandle->isInit())
        return true;
    m_mumuHandle->m_mumuPath = Settings::getSingletonSettings()->mumuPath();
    m_mumuHandle->m_mumuDisplayId = Settings::getSingletonSettings()->mumuDisplayId();
    m_mumuHandle->m_mumuInstIndex = Settings::getSingletonSettings()->mumuIndex();
    if (!m_mumuHandle->init())
        return false;
    m_displayBuffer.resize(m_mumuHandle->m_displayBufferSize);
    return true;
}
