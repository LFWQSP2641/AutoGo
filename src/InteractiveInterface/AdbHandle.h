#ifndef ADBHANDLE_H
#define ADBHANDLE_H

#include <QObject>

class QProcess;

class AdbHandle
{
public:
    static AdbHandle *instance();
    bool isAdbConnected() const;

    bool init();

    QByteArray executeAdbCommand(const QStringList &args);

protected:
    AdbHandle();
    ~AdbHandle();
    AdbHandle(const AdbHandle &) = delete;
    AdbHandle &operator=(const AdbHandle &) = delete;

public:
    QProcess *m_adbProcess;
    QString m_adbPath;
    QString m_adbSerial;

protected:
    static AdbHandle *m_instance;
    bool m_isAdbConnected;
};

#endif // ADBHANDLE_H
