#ifndef ADBSCREENCAPTOR_H
#define ADBSCREENCAPTOR_H

#include "AdbInteractor.h"
#include "Screencaptor.h"

class AdbScreencaptor : public Screencaptor, public AdbInteractor
{
    Q_OBJECT

public:
    explicit AdbScreencaptor(QObject *parent = nullptr);

    virtual bool init() override;

    virtual std::optional<QImage> screencap() override;
};

#endif // ADBSCREENCAPTOR_H
