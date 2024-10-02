#ifndef ADBSCREENCAPTOR_H
#define ADBSCREENCAPTOR_H

#include "Screencaptor.h"

class QProcess;

class AdbScreencaptor : public Screencaptor
{
    Q_OBJECT

public:
    explicit AdbScreencaptor(QObject *parent = nullptr);

    virtual bool init() override;

    virtual std::optional<cv::Mat> screencap() override;

protected:
    QProcess *adbProcess;
};

#endif // ADBSCREENCAPTOR_H
