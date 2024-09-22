#include "MumuScreencaptor.h"

#include <QDir>
#include <QFile>
#include <QLibrary>
#include <opencv2/opencv.hpp>

MumuScreencaptor::MumuScreencaptor(QObject *parent)
    : Screencaptor{ parent },
      library(new QLibrary(this))
{
}

MumuScreencaptor::~MumuScreencaptor()
{
    uninit();
}

std::optional<cv::Mat> MumuScreencaptor::screencap()
{
    if (m_nemuCaptureDisplayFunction == nullptr)
        return std::nullopt;
    int ret = m_nemuCaptureDisplayFunction(
        m_mumuHandle,
        m_mumuDisplayId,
        static_cast<int>(m_displayBuffer.size()),
        &m_displayWidth,
        &m_displayHeight,
        m_displayBuffer.data());
    if (ret)
        return std::nullopt;

    cv::Mat raw(m_displayHeight, m_displayWidth, CV_8UC4, m_displayBuffer.data());
    cv::Mat bgr;
    cv::cvtColor(raw, bgr, cv::COLOR_RGBA2BGR);
    cv::Mat dst;
    cv::flip(bgr, dst, 0);

    return dst;
}

bool MumuScreencaptor::init(const QString &mumuPath)
{
    QString dllPath(mumuPath);
    dllPath.append(QStringLiteral("/shell/sdk/external_renderer_ipc.dll"));
    if (!QFile(dllPath).exists())
        return false;
    m_mumuPath = mumuPath;
    library->setFileName(dllPath);
    typedef int (*NemuConnectType)(const wchar_t *, int);
    NemuConnectType nemuConnect = NemuConnectType(library->resolve("nemu_connect"));
    if (nemuConnect == nullptr)
        return false;
    std::wstring wstring(QDir::toNativeSeparators(mumuPath).toStdWString());
    m_mumuHandle = nemuConnect(wstring.c_str(), m_MumuInstIndex);
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
        return false;

    m_displayBuffer.resize(m_displayWidth * m_displayHeight * 4);

    return true;
}

bool MumuScreencaptor::reload()
{
    uninit();
    return init(m_mumuPath);
}

void MumuScreencaptor::uninit()
{
    m_nemuCaptureDisplayFunction = nullptr;
    library->unload();
}
