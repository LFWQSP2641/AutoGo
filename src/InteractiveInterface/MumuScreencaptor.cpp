#include "MumuScreencaptor.h"

#include "MumuHandle.h"
#include "src/Settings.h"

#include <QDir>
#include <QFile>
#include <QLibrary>
#include <opencv2/opencv.hpp>

MumuScreencaptor::MumuScreencaptor(QObject *parent)
    : Screencaptor{ parent },
      m_mumuHandle{ MumuHandle::getInstance() }
{
}

std::optional<cv::Mat> MumuScreencaptor::screencap()
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

    cv::Mat raw(m_mumuHandle->m_displayHeight, m_mumuHandle->m_displayWidth, CV_8UC4, m_displayBuffer.data());
    cv::Mat bgr;
    cv::cvtColor(raw, bgr, cv::COLOR_RGBA2BGR);
    cv::Mat dst;
    cv::flip(bgr, dst, 0);

    return dst;
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
