#ifndef MUMUSCREENCAPTOR_H
#define MUMUSCREENCAPTOR_H

#include "Screencaptor.h"

class MumuHandle;

class MumuScreencaptor : public Screencaptor
{
    Q_OBJECT

public:
    explicit MumuScreencaptor(QObject *parent = nullptr);

    virtual std::optional<QImage> screencap() override;

    virtual bool init() override;

protected:
    MumuHandle *m_mumuHandle;
    std::vector<unsigned char> m_displayBuffer;
};

#endif // MUMUSCREENCAPTOR_H
