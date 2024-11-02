#ifndef MUMUHANDLE_H
#define MUMUHANDLE_H

#include <QString>

class QLibrary;

class MumuHandle
{
public:
    static MumuHandle *getInstance();
    bool init();
    void uninit();

    bool isInit() const;

protected:
    MumuHandle();
    ~MumuHandle();
    MumuHandle(const MumuHandle &) = delete;
    MumuHandle &operator=(const MumuHandle &) = delete;

public:
    QString m_mumuPath;
    QLibrary *library;
    typedef int (*NemuCaptureDisplayType)(int, unsigned int, int, int *, int *, unsigned char *);
    NemuCaptureDisplayType m_nemuCaptureDisplayFunction = nullptr;
    int m_mumuInstIndex = 0;
    int m_mumuDisplayId = 0;
    int m_mumuHandle = 0;
    int m_displayWidth = 0;
    int m_displayHeight = 0;
    qsizetype m_displayBufferSize = 0;

protected:
    static MumuHandle *instance;
    bool inited = false;
};

#endif // MUMUHANDLE_H
