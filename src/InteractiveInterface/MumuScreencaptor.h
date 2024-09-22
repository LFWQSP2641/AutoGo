#ifndef MUMUSCREENCAPTOR_H
#define MUMUSCREENCAPTOR_H

#include "Screencaptor.h"

class QLibrary;

class MumuScreencaptor : public Screencaptor
{
    Q_OBJECT

public:
    explicit MumuScreencaptor(QObject *parent = nullptr);
    ~MumuScreencaptor();

    virtual std::optional<cv::Mat> screencap() override;

    bool init(const QString &mumuPath);
    bool reload();
    void uninit();

protected:
    QString m_mumuPath;
    QLibrary *library;
    typedef int (*NemuCaptureDisplayType)(int, unsigned int, int, int *, int *, unsigned char *);
    NemuCaptureDisplayType m_nemuCaptureDisplayFunction = nullptr;
    int m_MumuInstIndex = 1;
    int m_mumuDisplayId = 0;

    int m_mumuHandle = 0;
    int m_displayWidth = 0;
    int m_displayHeight = 0;
    std::vector<unsigned char> m_displayBuffer;
};

#endif // MUMUSCREENCAPTOR_H
